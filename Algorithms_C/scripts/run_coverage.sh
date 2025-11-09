#!/usr/bin/env bash
set -euo pipefail

script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
project_root=$(cd "${script_dir}/.." && pwd)
build_dir_default="${project_root}/../build/coverage"
BUILD_DIR=${BUILD_DIR:-${build_dir_default}}

mkdir -p "${BUILD_DIR}"
cmake -S "${project_root}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build "${BUILD_DIR}" --target coverage-report
