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
- Public exception hierarchy: a single `catch` covers everything, or you can
  catch the specific error and read its position.

### Not implemented

- Two- and three-variable functions. `v1` covers a single variable.
- Scientific notation (`1e3`).

## Usage

```cpp
#include <FunctionParser/FunctionParser.h>

FunctionParser::Expression expression("x*x");

auto value = expression.eval(2.0);          // std::optional<double> -> 4
auto points = expression.evaluateFunction({0.0, 10.0, 5});   // 6 points
```

## Syntax

A valid expression:
pi + e - 10


Available functions:
- pow(base, exponent) = base ^ exponent
- log(argument, base) = log_base(argument)
- nrt(radicand, index) = the index-th root of the radicand
- sin(x) / cos(x) / tan(x)


## Non-evaluable values

When the function has no value at a point — division by zero, logarithm of a
non-positive argument, root of a negative radicand — sampling still returns the
point, with its `y` coordinate set to `NaN`. Overflow yields `±inf`. The vector
always holds `steps + 1` elements, so the index matches the position along the
axis.

Checking each point before using it is the consumer's responsibility:

```cpp
for (const auto& point : points) {
    if (!std::isfinite(point.y)) continue;  // no value at this x
    // ...
}
```

Use `isfinite`, not `isnan`: overflow produces infinities, which `isnan` does
not catch. And never compare with `==`: `NaN == NaN` is false.

## Errors

Invalid input throws. Everything the library throws derives from
`FunctionParser::LibraryException`:

```cpp
try {
    FunctionParser::Expression expression("(1 + 2");
} catch (const FunctionParser::ExpressionError& e) {
    // e.what()     -> "There is an open parenthesis"
    // e.position   -> 0
} catch (const FunctionParser::RangeError& e) {
    // invalid Range: zero steps, min > max, non-finite bounds
}
```

`position` is `0` when the error does not map to a specific character.

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

Consuming it from another CMake project:

```cmake
include(FetchContent)
FetchContent_Declare(
    FunctionParser
    GIT_REPOSITORY https://github.com/MrAxelius/FunctionParser.git
    GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(FunctionParser)
target_link_libraries(your_target PRIVATE FunctionParser::FunctionParser)
```
### Known limitations

- `pow((1,2))` parses and returns 1 instead of reporting a syntax error.
- Evaluation is recursive, so deeply nested expressions can overflow the
  stack (around 7000 terms on Windows).
- Odd roots of negative numbers are rejected: `nrt(-8, 3)` returns no value
  even though it is defined over the reals. This is a deliberate
  simplification, not a mathematical claim.
- `nrt(x, i)` with a non-finite index returns 1, because `1/inf` is 0 and
  IEEE 754 defines `pow(x, 0)` as 1.
- `pow(x, 0)` returns 1 for every `x`, including NaN, and `pow(1, y)`
  returns 1 for every `y`, including NaN. Both are required by IEEE 754, so
  an error value that reaches either one stops propagating and the result
  looks valid.