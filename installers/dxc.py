import os
import subprocess
import sys
from package_helpers import clone_and_checkout, download_file, extract_zip, copy_file, run, append_paths

def main():
    build_mode = sys.argv[1]
    install_prefix = sys.argv[2]
    module_destination = sys.argv[3]

    if not (install_prefix and module_destination):
        print("Invalid build type or install path")
        return

    dxc_url = "https://github.com/microsoft/DirectXShaderCompiler.git"
    dxc_tag = "release-1.8.2502"
    dxc_zip_url = "https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.8.2502/dxc_2025_02_20.zip"

    package_output_dir = "dxc"
    dxc_src = append_paths(module_destination, "modules", package_output_dir)
    dxc_build = append_paths(module_destination, "dependencies", "windows", package_output_dir)
    dxc_zip = append_paths(module_destination, "dxil.zip")
    dxc_extract = append_paths(module_destination, "dxil")

    clone_and_checkout(dxc_url, destination=dxc_src, commit_hash=None, branch=dxc_tag)

    # Configure with CMake
    run(
        f'cmake -S "{dxc_src}" -B "{dxc_build}" -G Ninja '
        f'-C "{dxc_src}/cmake/caches/PredefinedParams.cmake" '
        f'-DCMAKE_BUILD_TYPE={build_mode} '
        f'-DENABLE_SPIRV_CODEGEN=ON '
        f'-DCMAKE_C_COMPILER=cl.exe -DCMAKE_CXX_COMPILER=cl.exe '
        f'-DCMAKE_INSTALL_PREFIX="{install_prefix}"'
    )

    # Build install targets
    for target in ["install-dxc-headers", "install-dxc", "install-dxcompiler"]:
        run(f'cmake --build "{dxc_build}" --config {build_mode} --target {target}')

    # Copy dxcompiler.lib
    dxcompiler_lib = os.path.join(dxc_build, "lib", "dxcompiler.lib")
    copy_file(dxcompiler_lib, os.path.join(install_prefix, "lib"))

    # Download and extract dxil.dll
    download_file(dxc_zip_url, dxc_zip)
    extract_zip(dxc_zip, dxc_extract)

    # Copy dxil.dll
    dxil_dll = os.path.join(dxc_extract, "bin", "x64", "dxil.dll")
    copy_file(dxil_dll, os.path.join(install_prefix, "bin"))

    print("DXC and DXIL setup completed.")

if __name__ == "__main__":
    try:
        main()
    except subprocess.CalledProcessError as e:
        print(f"Command failed: {e}", file=sys.stderr)
        sys.exit(1)