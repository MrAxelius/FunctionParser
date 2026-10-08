## v1.1.0 vs v1.0.1 — paired design

Two binaries run alternately; the median of the per-pair differences is
reported. 8 pairs per phase.

| phase  | v1.0.1 | v1.1.0 | median paired diff | pairs favouring v1.1.0 |
|--------|--------|--------|--------------------|------------------------|
| eval   | 83.8   | 83.4   | **-0.2 ns** (-0.2%) | 5 of 8, both signs |
| sample | 82.3   | 79.25  | **-3.15 ns** (-3.8%) | **8 of 8** |

`eval`: no measurable change. Removing the per-node std::optional from the
evaluator bought nothing. The branch was perfectly predicted and
std::optional<double> is trivially copyable and returned in registers, so
neither cost what the original estimate assumed. Three successive estimates
were 65%, then ~5%, then ~0%; each improvement in method shrank the effect,
which is the signature of an effect that was never there.

`sample`: a real 3.8%, with every pair in the same direction and the spread
of the differences never reaching zero. Both phases run in the same two
binaries, so no machine-wide explanation can move one and not the other.
What differs is the inner loop of Muestreo::muestrear, which lost a branch
per point, an optional to dereference, and one of its two push_back sites.

Caveat: 3.15 ns is about ten cycles, more than a predicted branch should
cost, so the branch is probably not the whole story. And a paired design
cancels drift over time but not fixed differences between two separately
compiled binaries, so an incidental code-layout contribution cannot be ruled
out from these numbers alone.