import subprocess
import sys
from package_helpers import clone_and_checkout, append_paths, run

def main():
    build_mode = sys.argv[1]
    install_prefix = sys.argv[2]
    module_destination = sys.argv[3]

    package_output_dir = "glm"
    commit_hash_release = "0af55ccecd98d4e5a8d1fad7de25ba429d60e863"
    glm_dir = append_paths(module_destination, "modules", package_output_dir)
    build_dir = append_paths(module_destination, "dependencies", "windows", package_output_dir)

    clone_and_checkout("https://github.com/g-truc/glm.git", destination=glm_dir, commit_hash=commit_hash_release)

    run(f'cmake -S "{glm_dir}" -B "{build_dir}" '
        f'-DCMAKE_INSTALL_PREFIX="{install_prefix}" '
        f'-DGLM_BUILD_LIBRARY=OFF '
        f'-DGLM_BUILD_TESTS=OFF '
        f'-DGLM_BUILD_INSTALL=ON')

    run(f'cmake --build "{build_dir}" --target install')

if __name__ == "__main__":
    try:
        main()
    except subprocess.CalledProcessError as e:
        print(f"Error while running command: {e}", file=sys.stderr)
        sys.exit(1)