import subprocess
import sys
from package_helpers import clone_and_checkout, append_paths, run
from vksuper import vk_branch

def main():
    build_mode = sys.argv[1]
    install_prefix = sys.argv[2]
    module_destination = sys.argv[3]

    package_output_dir = "vulkan_loader"
    vk_loader_dir = append_paths(module_destination, "modules", package_output_dir)
    build_dir = append_paths(module_destination, "dependencies", "windows", package_output_dir)

    clone_and_checkout("https://github.com/KhronosGroup/Vulkan-Loader.git", destination=vk_loader_dir, branch=vk_branch)

    run(f'cmake -S "{vk_loader_dir}" -B "{build_dir}" '
        f'-DCMAKE_INSTALL_PREFIX="{install_prefix}" '
        f'-DUPDATE_DEPS=ON '
        f'-DCMAKE_BUILD_TYPE="{build_mode}"')

    run(f'cmake --build "{build_dir}" --config {build_mode} --target install')

if __name__ == "__main__":
    try:
        main()
    except subprocess.CalledProcessError as e:
        print(f"Error while running command: {e}", file=sys.stderr)
        sys.exit(1)