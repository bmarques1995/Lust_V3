import subprocess
import sys
from package_helpers import clone_and_checkout, append_paths, run

def main():
    build_mode = sys.argv[1]
    install_prefix = sys.argv[2]
    module_destination = sys.argv[3]

    package_output_dir = "vma"
    commit_hash_release = "1d8f600fd424278486eade7ed3e877c99f0846b1"
    vma_dir = append_paths(module_destination, "modules", package_output_dir)
    build_dir = append_paths(module_destination, "dependencies", "windows", package_output_dir)

    clone_and_checkout("https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git", destination=vma_dir, commit_hash=commit_hash_release)

    run(f'cmake -S "{vma_dir}" -B "{build_dir}" '
        f'-DCMAKE_INSTALL_PREFIX="{install_prefix}" '
        f'-DBUILD_SHARED_LIBS=ON')

    run(f'cmake --build "{build_dir}" --config {build_mode} --target install')

if __name__ == "__main__":
    try:
        main()
    except subprocess.CalledProcessError as e:
        print(f"Error while running command: {e}", file=sys.stderr)
        sys.exit(1)