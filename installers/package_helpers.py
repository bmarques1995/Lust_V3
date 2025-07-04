import os
import subprocess
import urllib.request
import zipfile
import shutil
import re
from pathlib import Path

def append_paths(base_path, *paths):
    return os.path.join(base_path, *paths).replace("\\", "/")

def append_msvc_compiler_location(compiler_location):
    full_path = Path(compiler_location)
    compiler_dir = full_path.parent
    current_path_var = os.environ.get("PATH", "")
    os.environ["PATH"] = f"{compiler_dir};{current_path_var}"

def append_vs_ninja_host(compiler_location):
    vs_base_path = ""
    match = re.search(r"(.*Community|Enterprise|Professional)", compiler_location)
    if match:
        vs_base_path = match.group(1)
    else:
        print("Version not found in compiler location")
        return

    ninja_path = os.path.join(vs_base_path, "Common7", "IDE", "CommonExtensions", "Microsoft", "CMake", "Ninja")
    current_path = os.environ.get("PATH", "")
    os.environ["PATH"] = f"{ninja_path};{current_path}"

def run(cmd, cwd=None):
    print(f"> {cmd}")
    subprocess.check_call(cmd, shell=True, cwd=cwd)

def clone_and_checkout(repo, destination, commit_hash=None, branch=None, patch=None):
    branch_arg = f'--branch {branch}' if branch else ''
    if not os.path.exists(destination):
        run(f'git clone --recursive {branch_arg} {repo} "{destination}"')
    if commit_hash:
        run(f'git reset --hard {commit_hash}', cwd=destination)
    if patch:
        if os.path.exists(patch):
            run(f'git apply -v "{patch}"', cwd=destination)
        else:
            print(f"Patch file {patch} not found, skipping patch application.")

def download_file(url, destination):
    print(f"Downloading: {url}")
    urllib.request.urlretrieve(url, destination)

def extract_zip(zip_path, extract_to):
    print(f"Extracting: {zip_path}")
    with zipfile.ZipFile(zip_path, 'r') as zip_ref:
        zip_ref.extractall(extract_to)

def copy_file(src, dst_dir):
    os.makedirs(dst_dir, exist_ok=True)
    dst_path = os.path.join(dst_dir, os.path.basename(src))
    print(f"Copying: {src} -> {dst_path}")
    shutil.copy2(src, dst_path)