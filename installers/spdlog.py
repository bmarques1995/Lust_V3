import subprocess
import sys
from package_helpers import clone_and_checkout, append_paths, run

def main():
    build_mode = sys.argv[1]
    install_prefix = sys.argv[2]
    module_destination = sys.argv[3]

    package_output_dir = "spdlog"
    commit_hash_release = "f355b3d58f7067eee1706ff3c801c2361011f3d5"
    spdlog_dir = append_paths(module_destination, "modules", package_output_dir)
    build_dir = append_paths(module_destination, "dependencies", "windows", package_output_dir)

    clone_and_checkout("https://github.com/gabime/spdlog.git", destination=spdlog_dir, commit_hash=commit_hash_release)

    run(f'cmake -S "{spdlog_dir}" -B "{build_dir}" '
        f'-DCMAKE_INSTALL_PREFIX="{install_prefix}" '
        f'-DBUILD_SHARED_LIBS=ON')

    run(f'cmake --build "{build_dir}" --config {build_mode} --target install')

if __name__ == "__main__":
    try:
        main()
    except subprocess.CalledProcessError as e:
        print(f"Error while running command: {e}", file=sys.stderr)
        sys.exit(1)