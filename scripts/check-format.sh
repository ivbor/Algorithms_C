#!/usr/bin/env bash
set -euo pipefail

FILES=$(find include src tests -name '*.c' -o -name '*.h')
clang-format -n --Werror ${FILES}
