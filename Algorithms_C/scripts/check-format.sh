#!/usr/bin/env bash
set -euo pipefail

usage() {
    cat <<USAGE
Usage: $(basename "$0") [--apply]

Without arguments the script runs clang-format in check mode. Pass --apply to
rewrite files in place.
USAGE
}

mode="check"
if [[ $# -gt 1 ]]; then
    usage >&2
    exit 1
fi

if [[ $# -eq 1 ]]; then
    case "$1" in
        --apply)
            mode="apply"
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            usage >&2
            exit 1
            ;;
    esac
fi

script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
project_root=$(cd "${script_dir}/.." && pwd)

if ! find "${project_root}/include" "${project_root}/src" "${project_root}/tests" \
    -type f \( -name '*.c' -o -name '*.h' \) -print -quit >/dev/null 2>&1; then
    echo "No source files found" >&2
    exit 0
fi

if [[ "${mode}" == "apply" ]]; then
    find "${project_root}/include" "${project_root}/src" "${project_root}/tests" \
        -type f \( -name '*.c' -o -name '*.h' \) -print0 \
        | xargs -0 clang-format -i
else
    find "${project_root}/include" "${project_root}/src" "${project_root}/tests" \
        -type f \( -name '*.c' -o -name '*.h' \) -print0 \
        | xargs -0 clang-format -n --Werror
fi
