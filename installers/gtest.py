import subprocess
import sys
from package_helpers import clone_and_checkout, append_paths, run

def main():
    build_mode = sys.argv[1]
    install_prefix = sys.argv[2]
    module_destination = sys.argv[3]

    package_output_dir = "gtest"
    commit_hash_release = "52eb8108c5bdec04579160ae17225d66034bd723"
    gtest_dir = append_paths(module_destination, "modules", package_output_dir)
    build_dir = append_paths(module_destination, "dependencies", "windows", package_output_dir)

    clone_and_checkout("https://github.com/google/googletest.git", destination=gtest_dir, commit_hash=commit_hash_release)

    run(f'cmake -S "{gtest_dir}" -B "{build_dir}" '
        f'-DCMAKE_INSTALL_PREFIX="{install_prefix}" '
        f'-DCMAKE_BUILD_TYPE="{build_mode}" '
        f'-DBUILD_SHARED_LIBS=ON '
        f'-DCMAKE_C_COMPILER=cl '
        f'-DCMAKE_CXX_COMPILER=cl')

    run(f'cmake --build "{build_dir}" --config {build_mode} --target install')

if __name__ == "__main__":
    try:
        main()
    except subprocess.CalledProcessError as e:
        print(f"Error while running command: {e}", file=sys.stderr)
        sys.exit(1)