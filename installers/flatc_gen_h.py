#!/usr/bin/env python3
import subprocess
import sys
import os
from pathlib import Path
import argparse
from package_helpers import run

def run_flatc(fbs_file: Path, fbs_path: Path, flatc_path: str = "flatc"):
    print(f"[flatc] Generating header for: {fbs_file}")
    run(f'{flatc_path} -o {fbs_path} --cpp {fbs_file}')

def collect_fbs_files(paths):
    fbs_files = []
    for path in paths:
        p = Path(path)
        if p.is_file() and p.suffix == ".fbs":
            fbs_files.append(p)
        elif p.is_dir():
            fbs_files.extend(p.glob("*.fbs"))
    return fbs_files

def main():
    install_prefix = sys.argv[1]
    fbs_dir = sys.argv[2]
    fbs_files = sys.argv[3]
    #branch_arg = f'--branch {branch}' if branch else ''
    fbs_file_list = fbs_files.split(";") if ";" in fbs_files else [fbs_files]
    flatc_path = os.path.join(install_prefix, "bin", "flatc")

    if not fbs_files:
        print("No .fbs files found.")
        sys.exit(1)

    for fbs_file in fbs_file_list:
        run_flatc(fbs_file, fbs_dir, flatc_path)

if __name__ == "__main__":
    main()