from package_helpers import clone_and_checkout, run, append_vs_ninja_host, append_paths

def build_qt_package(name, version, build_mode, install_prefix, module_destination, vs_compiler):
    if not (build_mode and install_prefix and module_destination and vs_compiler):
        print("Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination")
        return

    append_vs_ninja_host(vs_compiler)

    print(f"Installing {name} with build mode: {build_mode}, install prefix: {install_prefix}, module destination: {module_destination}")

    qt_package_src = append_paths(module_destination, "modules", name)
    qt_package_build = append_paths(module_destination, "dependencies", "windows", name)

    clone_and_checkout(f"https://code.qt.io/qt/{name}.git", destination=qt_package_src, branch=version)

    run(
        f'cmake -S "{qt_package_src}" -B "{qt_package_build}" -G Ninja '
        f'-DQT_FEATURE_force_bundled_libs=ON '
        f'-DCMAKE_INSTALL_PREFIX="{install_prefix}" '
        f'-DCMAKE_PREFIX_PATH="{install_prefix}" '
        f'-DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl -DCMAKE_ASM_COMPILER=ml64 '
        f'-DCMAKE_BUILD_TYPE="{build_mode}"'
    )

    run(f'cmake --build "{qt_package_build}" --parallel')
    run(f'cmake --install "{qt_package_build}"')