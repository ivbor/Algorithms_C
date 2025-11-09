#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR=${BUILD_DIR:-build/coverage}
mkdir -p "${BUILD_DIR}"
cmake -S . -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build "${BUILD_DIR}" --target coverage-report
