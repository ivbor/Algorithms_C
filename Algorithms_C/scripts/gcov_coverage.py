#!/usr/bin/env python3
"""Generate basic GCOV coverage reports without gcovr.

This script is intentionally verbose, mirroring the documentation-first ethos
of the repository.  It enumerates every ``.gcno`` file inside the supplied
object directory, invokes ``gcov`` to materialize ``.gcov`` files into a
scratch directory, and then aggregates line/branch coverage metrics.  The
result mirrors the high-level reports emitted by ``gcovr`` so that the
``coverage-report`` CMake target keeps passing even on systems where ``gcovr``
is not installed.
"""
from __future__ import annotations

import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys
from typing import Iterable, List, Tuple


class CoverageError(RuntimeError):
    """Raised when coverage artifacts cannot be generated."""


def _run_gcov(gcno_files: Iterable[Path], output_dir: Path) -> None:
    """Invoke ``gcov`` for every ``.gcno`` file, writing ``.gcov`` outputs."""

    for gcno in gcno_files:
        cmd = [
            "gcov",
            "--branch-counts",
            "--branch-probabilities",
            "--preserve-paths",
            str(gcno),
        ]
        result = subprocess.run(
            cmd,
            cwd=output_dir,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            check=False,
        )
        if result.returncode != 0:
            raise CoverageError(
                f"gcov failed for {gcno} with exit code {result.returncode}:\n{result.stdout}"
            )


def _aggregate(gcov_files: Iterable[Path]) -> Tuple[float, float, List[str]]:
    """Aggregate line/branch totals from the generated ``.gcov`` files."""

    total_lines = covered_lines = 0.0
    total_branches = covered_branches = 0.0
    per_file_rows: List[str] = []

    for gcov_file in gcov_files:
        lines = gcov_file.read_text(encoding="utf-8", errors="ignore").splitlines()
        source_line = next((ln for ln in lines if "Source:" in ln), None)
        if source_line and "/tests/" in source_line:
            continue
        if source_line and "minunit.c" in source_line:
            continue
        file_total = file_covered = 0.0
        file_branch_total = file_branch_covered = 0.0

        for raw_line in lines:
            stripped = raw_line.strip()
            if stripped.startswith("branch"):
                file_branch_total += 1
                if "never executed" in stripped:
                    continue
                if "taken 0" in stripped:
                    continue
                file_branch_covered += 1
                continue
            if stripped.startswith("call"):
                continue

            parts = raw_line.split(":", 2)
            if len(parts) < 3:
                continue
            count_field, line_no_field, _ = parts
            line_no_field = line_no_field.strip()
            if not line_no_field.isdigit():
                continue
            if int(line_no_field) == 0:
                continue

            count_field = count_field.strip()
            if not count_field or count_field == "-":
                continue
            if count_field == "=====":
                continue

            file_total += 1
            if count_field == "#####":
                continue
            try:
                executed = int(count_field)
            except ValueError:
                continue
            if executed > 0:
                file_covered += 1

        if file_total == 0:
            continue

        total_lines += file_total
        covered_lines += file_covered
        total_branches += file_branch_total
        covered_branches += file_branch_covered

        line_pct = (file_covered / file_total) * 100.0
        branch_pct = 0.0
        if file_branch_total:
            branch_pct = (file_branch_covered / file_branch_total) * 100.0
        per_file_rows.append(
            f"<tr><td>{gcov_file.name}</td><td>{line_pct:.2f}%</td><td>{branch_pct:.2f}%</td></tr>"
        )

    if total_lines == 0:
        raise CoverageError("No executable lines were discovered in gcov output.")

    line_rate = covered_lines / total_lines
    branch_rate = 0.0
    if total_branches > 0:
        branch_rate = covered_branches / total_branches

    return line_rate, branch_rate, per_file_rows


def _write_reports(
    *,
    line_rate: float,
    branch_rate: float,
    output_txt: Path,
    output_html: Path,
    rows: List[str],
) -> None:
    """Emit human-friendly text and HTML summaries."""

    summary = (
        f"Overall line coverage : {line_rate * 100:.2f}%\n"
        f"Overall branch coverage: {branch_rate * 100:.2f}%\n"
    )
    output_txt.write_text(summary, encoding="utf-8")

    html = f"""
<!DOCTYPE html>
<html lang=\"en\">
<head>
<meta charset=\"utf-8\" />
<title>Algorithms_C Coverage Report</title>
<style>
body {{ font-family: sans-serif; margin: 2rem; }}
table {{ border-collapse: collapse; width: 100%; }}
th, td {{ border: 1px solid #ccc; padding: 0.5rem; text-align: left; }}
th {{ background: #f4f4f4; }}
</style>
</head>
<body>
<h1>Algorithms_C Coverage Report</h1>
<p>Overall line coverage: {line_rate:.2%}</p>
<p>Overall branch coverage: {branch_rate:.2%}</p>
<table>
<thead><tr><th>File</th><th>Line %</th><th>Branch %</th></tr></thead>
<tbody>
{os.linesep.join(rows)}
</tbody>
</table>
</body>
</html>
"""
    output_html.write_text(html, encoding="utf-8")


def main(argv: List[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Minimal gcov coverage aggregator")
    parser.add_argument("--root", required=True, type=Path)
    parser.add_argument("--object-dir", required=True, type=Path)
    parser.add_argument("--output-dir", required=True, type=Path)
    parser.add_argument("--txt", required=True, type=Path)
    parser.add_argument("--html", required=True, type=Path)
    parser.add_argument("--min-line-rate", type=float, default=0.8)
    parser.add_argument("--min-branch-rate", type=float, default=0.0)
    args = parser.parse_args(argv)

    output_dir = args.output_dir
    if output_dir.exists():
        shutil.rmtree(output_dir)
    output_dir.mkdir(parents=True)

    gcno_files = sorted(args.object_dir.rglob("*.gcno"))
    if not gcno_files:
        raise CoverageError("No .gcno files found. Did you enable coverage instrumentation?")

    _run_gcov(gcno_files, output_dir)
    line_rate, branch_rate, rows = _aggregate(sorted(output_dir.glob("*.gcov")))
    _write_reports(
        line_rate=line_rate,
        branch_rate=branch_rate,
        output_txt=args.txt,
        output_html=args.html,
        rows=rows,
    )

    if line_rate < args.min_line_rate or branch_rate < args.min_branch_rate:
        raise CoverageError(
            f"Coverage below threshold. line={line_rate:.2%}, branch={branch_rate:.2%}"
        )

    print(
        f"Line coverage {line_rate:.2%}, branch coverage {branch_rate:.2%}. "
        f"Reports saved to {args.txt} and {args.html}"
    )
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except CoverageError as exc:
        print(f"[coverage] {exc}", file=sys.stderr)
        raise SystemExit(1)
