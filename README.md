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
7. [License](#license)

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
The helper script configures a dedicated build directory and produces HTML and XML
reports using `gcovr`.  Non-functional helper code (unit tests and the bundled
`minunit` framework) is excluded so that the reported percentage focuses on the
library itself:

```bash
Algorithms_C/scripts/run_coverage.sh
```

The resulting report lives at `build/coverage/coverage.html` and is uploaded as an
artifact in CI.

## Implemented modules

The initial translation pass currently includes:

* Generic binary search helpers (`ac_binary_search`, `ac_lower_bound`, `ac_upper_bound`).
* Sorting algorithms – insertion sort, merge sort, quick sort, and counting sort –
  matching the Python implementations.
* Counting sort for two-dimensional arrays (`ac_array_count_sort`).
* Core data structures: a generic `ac_vector`, stack (`ac_stack`), queue (`ac_queue`),
  double-ended queue (`ac_deque`), and matrix printing helpers (`ac_print_matrix`).
* Utility helpers for comparisons and logging.

Each module is unit-tested and, where appropriate, covered by stress harnesses.
The structure headers mirror the Python repository's rich docstrings so that
every function now carries both short and long-form descriptions directly in the
code, making the C translation an equally verbose learning resource.

## Continuous integration

GitHub Actions builds the project on Linux, macOS, and Windows.  Separate jobs
run clang-format/clang-tidy, execute the unit and stress test suites, and publish
coverage reports.

## License

The original Python material is licensed under the repository owner's terms.
This translation follows the same intent for educational use.
