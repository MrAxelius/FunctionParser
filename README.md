# FunctionParser

A C++20 library for parsing and evaluating single-variable mathematical
expressions.

The main goal is learning API and library design. It also serves as a component
of [Function-render](https://github.com/MrAxelius/Function-render), a function
viewer.

## Features

- Lexical and syntactic analysis with positioned errors.
- AST construction with operator precedence, basic operators (`+`, `-`, `*`, `/`)
  and functions.
- Unary minus, numbers, constants (`pi`, `e`), parentheses and nested functions.
- Recursive evaluation and sampling over a range.
- Public facade using pImpl, versioned through an `inline namespace`.
- Public exception hierarchy: a single `catch` covers every input error, or you
  can catch the specific one and read its position.

### Not implemented

- Two- and three-variable functions. `v1` covers a single variable.
- Scientific notation (`1e3`).

## Usage

```cpp
#include <FunctionParser/FunctionParser.h>

FunctionParser::Expression expression("x*x");

auto value = expression.eval(2.0);                           // optional<double> -> 4
auto points = expression.evaluateFunction({0.0, 10.0, 5});   // 6 points
```

## Syntax

```
pi + e - 10
3*x*x - 2*x + 1
sin(cos(x)) / pow(x, 2)
```

Available functions:

- `pow(base, exponent)` — base raised to exponent
- `log(argument, base)` — logarithm of argument in the given base
- `nrt(radicand, index)` — the index-th root of the radicand
- `sin(x)`, `cos(x)`, `tan(x)`

## Results that are not numbers

The two entry points answer differently on purpose, and the difference is the
central design decision of the library.

### `eval` returns a finite number or nothing

```cpp
auto value = expression.eval(x);
if (!value) { /* no result at this x */ }
```

The empty optional covers every reason a result can fail to be a number:
a domain error (logarithm of a non-positive argument, root of a negative
radicand, base zero in a logarithm) and overflow alike. The engine computes
with plain IEEE 754 semantics and the check happens once, at the boundary, so
a caller never receives a NaN or an infinity from `eval`.

Before `v1.1.0` this was inconsistent: domain errors returned an empty
optional, while an overflow came back as an infinity *with* a value. If you
relied on that, see the migration note below.

### `evaluateFunction` returns the raw value, non-finite ones included

```cpp
for (const auto& point : points) {
    if (!std::isfinite(point.y)) continue;   // no value at this x
    // ...
}
```

Sampling never drops a point: the vector always holds exactly `steps + 1`
elements, so an index matches a position along the axis. Where the function has
no value, `y` carries what IEEE 754 produced:

- operations with no defined result give **NaN** — `log(-5, 2)`, `nrt(-8, 2)`
- quantities growing without bound give **±infinity** — `1/x` at zero,
  an overflow such as `pow(10, 400)`

Both are non-finite, so one `isfinite` test covers both. The distinction is
kept rather than normalised because the sign of an infinity says which way an
asymptote runs, which a plotter can use and a NaN would throw away.

Use `isfinite`, not `isnan`: infinities are not NaN. And never compare with
`==`: `NaN == NaN` is false.

**A finite value is not a promise of continuity.** Sampling `1/x` across zero
without landing exactly on it gives two large finite values of opposite sign,
and nothing in the data marks the pole. A consumer that draws line segments
needs its own jump threshold; `isfinite` cannot detect this case.

## Errors

Invalid **input** throws, and everything thrown for an invalid input derives
from `FunctionParser::LibraryException`:

```cpp
try {
    FunctionParser::Expression expression("(1 + 2");
} catch (const FunctionParser::ExpressionError& e) {
    // e.what()     -> "There is an open parenthesis"
    // e.position   -> 0
} catch (const FunctionParser::RangeError& e) {
    // invalid Range: zero steps, too many steps, min > max, non-finite bounds
}
```

`position` is `0` when the error does not map to a specific character.

Violations of the library's own internal invariants are a separate matter: they
are thrown as types outside this hierarchy and mean there is a bug in the
library, not in your input. A correct program never sees one, and catching
`LibraryException` is not meant to cover them.

## Validity

A freshly constructed `Expression` is always valid: the constructor either
succeeds or throws. Moving from an `Expression` leaves the source invalid.

`eval` and `evaluateFunction` require a valid object. Calling either one on an
invalid `Expression` is undefined behaviour.

Everything else is safe on any `Expression`, valid or not: the destructor, move
assignment, and the validity check itself.

```cpp
FunctionParser::Expression a("x*x");
FunctionParser::Expression b = std::move(a);

if (a) { /* not taken: a was moved from */ }
if (b) { /* taken */ }
```

You only need the check where an `Expression` may have been moved from. A
freshly constructed one does not.

## Building

Requires CMake 3.20 or newer and a C++20 compiler. Catch2 is fetched
automatically when tests are enabled.

```bash
cmake -B build -DBUILD_SHARED_LIBS=OFF
cmake --build build
ctest --test-dir build --output-on-failure
```

On a multi-config generator, pass the configuration to both steps:

```bash
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

### Build options

| option | default | effect |
|---|---|---|
| `BUILD_TESTS` | `ON` | builds `parser_tests` and `api_tests`, fetches Catch2 |
| `BUILD_EXAMPLES` | `OFF` | builds the CLI example |
| `FUNCTIONPARSER_WERROR` | `OFF` | warnings as errors; CI builds with it on |
| `FUNCTIONPARSER_BUILD_BENCHMARKS` | `OFF` | builds `fp_benchmark` |

Consuming it from another CMake project:

```cmake
include(FetchContent)
FetchContent_Declare(
    FunctionParser
    GIT_REPOSITORY https://github.com/MrAxelius/FunctionParser.git
    GIT_TAG v1.1.0
)
FetchContent_MakeAvailable(FunctionParser)
target_link_libraries(your_target PRIVATE FunctionParser::FunctionParser)
```

`find_package(FunctionParser)` is **not** supported yet: the install rules copy
the headers but do not export the target.

## Migrating to v1.1.0

The only behaviour change is in `eval`, and only for overflow:

| expression | v1.0.1 | v1.1.0 |
|---|---|---|
| domain error, e.g. `log(-5, 2)` | empty optional | empty optional |
| overflow, e.g. `pow(10, 400)` | `inf`, with a value | **empty optional** |

If you were calling `eval(x).value_or(0)` on an expression that overflows, you
used to get an infinity and now get `0`.

`evaluateFunction` keeps the same contract — one point per sample, non-finite
values included — but not the same values in every case: where `v1.0.1` wrote a
NaN for anything the evaluator rejected, the raw value now comes through, so an
asymptote arrives as `±inf` and keeps its sign. Both are non-finite, so a
consumer testing `isfinite` sees no difference.

## Performance

Measured figures and, more importantly, the measurement method and its limits
are in [benchmark/RESULTS.md](benchmark/RESULTS.md). The short version: removing
the per-node `std::optional` in `v1.1.0` produced **no measurable change** in
`eval` and a real 3.8% in sampling. The benchmark is a CMake target so the
numbers can be reproduced.

```bash
cmake -B build -DFUNCTIONPARSER_BUILD_BENCHMARKS=ON
cmake --build build --config Release
./build/fp_benchmark eval
```

## Known limitations

- `pow((1,2))` parses and returns 1 instead of reporting a syntax error.
- Evaluation is recursive, so deeply nested expressions can overflow the stack
  (around 7000 terms on Windows).
- Odd roots of negative numbers are rejected: `nrt(-8, 3)` returns no value even
  though it is defined over the reals. A deliberate simplification, not a
  mathematical claim.
- `nrt(x, i)` with a non-finite index returns 1, because `1/inf` is 0 and
  IEEE 754 defines `pow(x, 0)` as 1.
- `pow(x, 0)` returns 1 for every `x`, including NaN, and `pow(1, y)` returns 1
  for every `y`, including NaN. Both are required by IEEE 754, so an error value
  that reaches either one stops propagating and the result looks valid.
- No `find_package` support, as noted under Building.

## Planned

An intermediate representation — a flat instruction array replacing the pointer
based AST — is designed but not implemented. A sampling profile of `v1.0.1` put
31.6% of the time in the tree walk and 17.4% in `RtlAllocateHeap` and
`RtlFreeHeap`, which is the target: one allocation instead of one per node, and
contiguous memory instead of a scattered tree. It is not currently the active
line of work.