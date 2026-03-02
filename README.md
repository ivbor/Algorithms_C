# Algorithms_C

A modernised C translation of the data structures and algorithms from
[Algorithms_Python](https://github.com/ivbor/Algorithms_Python).  The codebase is
organised as a reusable static library accompanied by examples, unit tests, stress
harnesses, and CI automation.

## Table of contents

1. [Repository layout](#repository-layout)
2. [Building](#building)
3. [Usage](#usage)
4. [Coverage](#coverage)
5. [Implemented modules](#implemented-modules)
6. [Continuous integration](#continuous-integration)
7. [Documentation style](#documentation-style)
8. [License](#license)

## Repository layout

```
Algorithms_C/
  include/algorithms_c/  Public headers grouped by domain (algorithms, structures, utils)
  src/                   Library implementation sources
  tests/unit/            Unit test suites using the bundled minunit framework
  tests/stress/          Deterministic stress harnesses for heavy workloads
  scripts/               Tooling helpers (formatting, coverage)
  examples/              Translated dynamic-programming examples from the Python repo
.github/workflows/       CI pipelines (linting, build/test, coverage)
```

Quick links:

* [Headers](Algorithms_C/include/algorithms_c)
* [Sources](Algorithms_C/src)
* [Unit tests](Algorithms_C/tests/unit)
* [Stress harnesses](Algorithms_C/tests/stress)
* [Utility scripts](Algorithms_C/scripts)

## Building

The project uses CMake (and optionally Ninja).  A convenience `Makefile` is
provided, so the typical workflow is simply:

```bash
make              # configure and build (defaults to ./build)
make algorithms   # build only the algorithms static library
make structures   # build only the structures static library
make utils        # build only the shared utilities
make test         # execute the minunit-based test suite
make stress       # build and run long-running stress checks
make tidy         # run clang-tidy with the repository configuration
make format       # apply clang-format to the C sources and headers
```

To generate an IDE-friendly build directory manually:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Usage

All public headers reside under `Algorithms_C/include/algorithms_c`.  A minimal
program that sorts a `vector` looks like:

```c
#include "algorithms_c/algorithms/compare.h"
#include "algorithms_c/algorithms/sorting.h"
#include "algorithms_c/structures/vector.h"

int main(void) {
    ac_vector vec;
    ac_vector_from_array(&vec, sizeof(int), (int[]){4, 2, 1, 3}, 4);
    ac_quick_sort(ac_vector_data(&vec), ac_vector_size(&vec), sizeof(int), ac_compare_int);
    ac_vector_destroy(&vec);
    return 0;
}
```

From CMake you can link against the interface target exposed by this project:

```cmake
find_package(AlgorithmsC CONFIG REQUIRED)
add_executable(sample main.c)
target_link_libraries(sample PRIVATE AlgorithmsC::algorithms_c)
```

## Coverage

Coverage instrumentation is available through the `ENABLE_COVERAGE` CMake option.
The helper script configures a dedicated build directory and produces HTML and
text reports.  When `gcovr` is installed it is used directly, otherwise the
repository falls back to `scripts/gcov_coverage.py`, which drives `gcov` and
aggregates the same headline metrics.  Non-functional helper code (unit tests
and the bundled `minunit` framework) is excluded so that the reported percentage
focuses on the library itself:

```bash
Algorithms_C/scripts/run_coverage.sh
```

The resulting report lives at `build/coverage/coverage.html` and is uploaded as an
artifact in CI.

## Implemented modules

The initial translation pass currently includes:

* Generic search helpers: binary search/bounds (`ac_binary_search`, `ac_lower_bound`, `ac_upper_bound`).
  * Source structure translated in this step: `Algorithms_Python/bin_search.py`
    (mirrored by `ac_bin_search_int` in `Algorithms_C/include/algorithms_c/algorithms/binary_search.h`
    and `Algorithms_C/src/algorithms/binary_search.c`).
* Integer duplicate-range bound helpers (`ac_bounds_lower_int`, `ac_bounds_upper_int`).
  * Source structure translated in this step: `Algorithms_Python/bounds.py`
    (mirrored by `Algorithms_C/include/algorithms_c/algorithms/bounds.h`
    and `Algorithms_C/src/algorithms/bounds.c`).
* Ternary search helpers for locating minima and maxima of unimodal functions
  (`ac_ternary_search_min`, `ac_ternary_search_max`).
* Selection helper that returns the k-th element without fully sorting the input
  (`ac_split_find`).
* Damerau-Levenshtein edit distance helper for strings
  (`ac_damerau_levenshtein_distance`, compatibility alias `ac_damerau_levenstein_distance`).
  * Wrapper parity is validated by unit test `tests/unit/test_damerau_example_wrapper.c` against the translated example source.
  * Source structure translated: `Algorithms_Python/examples/dp_solutions_from_page/damerau_levenstein.py` (algorithm) and mirrored educational wrapper `Algorithms_C/examples/dp_solutions_from_page/damerau_levenstein.c`.
* Knapsack instance generator helpers (`ac_genhard_generate_uncorrelated`, `ac_genhard_generate_weakly_correlated`, `ac_genhard_generate_strongly_correlated`).
  * Source structure translated in this step: `Algorithms_Python/examples/dp_solutions_from_page/genhard.py` mirrored as reusable C API (`Algorithms_C/include/algorithms_c/algorithms/genhard.h`, `Algorithms_C/src/algorithms/genhard.c`).
* 0/1 knapsack dynamic-programming helper (`ac_minknap_max_profit`).
  * Added `ac_minknap` compatibility wrapper preserving classic `(n, p, w, x, c)` call form for translated examples.
  * Source structure translated in this step: `Algorithms_Python/examples/dp_solutions_from_page/minknap.py` mirrored as reusable C API (`Algorithms_C/include/algorithms_c/algorithms/minknap.h`, `Algorithms_C/src/algorithms/minknap.c`).
* Sorting algorithms – insertion sort, bubble sort, selection sort, shell sort, comb sort, merge sort, quick sort, counting sort, and digit sort –
  matching the Python implementations.
  * Source structure translated in this step: `Algorithms_Python/count_sort.py`
    (mirrored by `ac_count_sort_int_auto` in `Algorithms_C/include/algorithms_c/algorithms/sorting.h`
    and `Algorithms_C/src/algorithms/counting_sort.c`).
  * Source structure translated in this step: ``Algorithms_Python/digit_sort.py``
    (mirrored by ``ac_digit_sort_int`` and ``ac_digit_sort_opt_int`` in ``Algorithms_C/include/algorithms_c/algorithms/sorting.h``
    and ``Algorithms_C/src/algorithms/digit_sort.c``).
  * Source structure translated in this step: `Algorithms_Python/insert_sort.py` helpers
    (mirrored by `ac_insertion_sort_double`, `ac_bin_search_double`, and `ac_insertion_sort_opt_double` in
    `Algorithms_C/include/algorithms_c/algorithms/sorting.h` and `Algorithms_C/src/algorithms/insertion_sort.c`).
  * Source structure translated in this step: Python repository `BubbleSort` routine
    (mirrored by `ac_bubble_sort` in `Algorithms_C/include/algorithms_c/algorithms/sorting.h`
    and `Algorithms_C/src/algorithms/bubble_sort.c`).
  * Source structure translated in this step: Python repository `SelectionSort` routine
    (mirrored by `ac_selection_sort` in `Algorithms_C/include/algorithms_c/algorithms/sorting.h`
    and `Algorithms_C/src/algorithms/selection_sort.c`).
  * Source structure translated in this step: `Algorithms_Python/quick_sort.py` numeric helper
    (mirrored by `ac_quick_sort_double` in `Algorithms_C/include/algorithms_c/algorithms/sorting.h`
    and `Algorithms_C/src/algorithms/quick_sort.c`).
  * Source structure translated in this step: Python repository `ShellSort` routine
    (mirrored by `ac_shell_sort` in `Algorithms_C/include/algorithms_c/algorithms/sorting.h`
    and `Algorithms_C/src/algorithms/shell_sort.c`).
  * Source structure translated in this step: `Algorithms_Python/algorithms/sorting/comb_sort.py`
    (mirrored by `ac_comb_sort` in `Algorithms_C/include/algorithms_c/algorithms/sorting.h`
    and `Algorithms_C/src/algorithms/comb_sort.c`).
* Counting sort for two-dimensional arrays (`ac_array_count_sort`).
* Core data structures: a generic `ac_vector`, stack (`ac_stack`), queue (`ac_queue`),
  * Stack docs now include expanded short/long descriptions and explicit signatures in both header and source commentary to match the documentation-first translation style.
  * Source structure translated in this step: Python repository `Stack` structure (mirrored by `ac_stack` in `Algorithms_C/include/algorithms_c/structures/stack.h` and `Algorithms_C/src/structures/stack.c`).
  * Matrix-view helper docs were also expanded with short/long descriptions and formatting rationale to mirror Python-style educational prose.
  * Queue source now includes explicit short/long/signature inline blocks for each operation to document FIFO/ring-buffer decisions in implementation context.
  * Source structure translated in this step: Python repository `Queue` structure (mirrored by `ac_queue` in `Algorithms_C/include/algorithms_c/structures/queue.h` and `Algorithms_C/src/structures/queue.c`).
  * Deque source now includes explicit short/long/signature inline blocks for each operation, documenting ring-buffer invariants and end-operation symmetry.
  * Source structure translated in this step: Python repository `Deque` structure (mirrored by `ac_deque` in `Algorithms_C/include/algorithms_c/structures/deque.h` and `Algorithms_C/src/structures/deque.c`).
  * Heap source now includes explicit short/long/signature inline blocks for each operation, documenting sift-up/sift-down and vector-backed storage decisions.
  * Source structure translated in this step: Python repository `Heap` structure (mirrored by `ac_heap` in `Algorithms_C/include/algorithms_c/structures/heap.h` and `Algorithms_C/src/structures/heap.c`).
  * Vector source now includes explicit short/long/signature inline blocks for each operation, documenting dynamic-array invariants and memory-growth decisions.
  * Source structure translated in this step: Python repository `Vector` structure (mirrored by `ac_vector` in `Algorithms_C/include/algorithms_c/structures/vector.h` and `Algorithms_C/src/structures/vector.c`).
  * Singly linked list source now includes explicit short/long/signature inline blocks for each operation, documenting node-link invariants and traversal-based decisions.
  * Source structure translated in this step: Python repository `LinkedList` structure (mirrored by `ac_slist` in `Algorithms_C/include/algorithms_c/structures/slist.h` and `Algorithms_C/src/structures/slist.c`).
  double-ended queue (`ac_deque`), singly linked list (`ac_slist`), binary
  min-heap (`ac_heap`), and matrix printing helpers (`ac_print_matrix`).
* Sorting helpers now include a heap-driven variant (`ac_heap_sort`) alongside
  insertion, merge, quick, and counting sort.
* Real-number binary search for monotonic functions (`ac_real_binary_search`).
  * Source structure translated in this step: `Algorithms_Python/real_bin_search.py`
    (mirrored by `ac_real_binary_search` in `Algorithms_C/include/algorithms_c/algorithms/real_binary_search.h`
    and `Algorithms_C/src/algorithms/real_binary_search.c`).
* Utility helpers for comparisons and logging.

Each module is unit-tested and, where appropriate, covered by stress harnesses.
The structure headers mirror the Python repository's rich docstrings so that
every function now carries both short and long-form descriptions directly in the
code, making the C translation an equally verbose learning resource.


## Documentation style

To stay faithful to the educational Python source, public headers and structure
implementations include short and long-form descriptions, repeated signatures,
and inline comments that explain invariants and design decisions (often in more
lines than the functional code itself). This is intentional: the C translation
prioritises readability and teaching value over terseness.

## Continuous integration

GitHub Actions builds the project on Linux, macOS, and Windows.  Separate jobs
run clang-format/clang-tidy, execute the unit and stress test suites, and publish
coverage reports.

## License

The original Python material is licensed under the repository owner's terms.
This translation follows the same intent for educational use.
