# Render Sample

This is a sample project for future projects to practice Graphics Programming in Vulkan and D3D12
Because the zero planning of a really simple engine is hard, I'm creating this sample to be later
used in a real Game Engine.

Considering that D3D12 and Vulkan are 2 of the 3 most complex graphics API, this repository will
help later on more complex features development.

# Download

To download this repository just run `git clone --recursive https://github.com/bmarques1995/Render.git` or,
if you've forgotten to recursively clone, run in the repository `git submodule update --init --recursive`

# Build

This repository is built under cmake and is structured in following distribution (each `-` indicates
one level dir):

- `-cmake_helpers`: A submodule with a set of cmake helpers, going from dependencies tracing to installers
- `-installers`: A set of PowerShell, and futurely Bash, scripts to download the dependencies, for a real project, i will use an own multiplatform script language
- `-Lust`: Engine core library, all the render techniques
- `-ShaderManager`: Library renponsible to compile shaders and generate metadata
- `-Sandbox`: Library for behavior tests.
- `-Launcher`: Executable, encapsulates all the application
- `-Utils`: Library with utilities, like the console or a file I/O

To build the repository, just select a preset, at this point, only the presets `x64-debug` and `x64-release`
are avaliable, and to build, just run `cmake --preset <preset_name>`, that all dependencies will be installed
and the project will be built.

Disclaimer: all the installers will be run at first time by cmake, so, the first build will take a several time,
and they will be build and install all programs. But, if the dependencies are installed in the prefix_path, the build
will be quick.

Disclaimer 2: you will need admin privileges on windows to run the vulkan version, it will record in HKEY_LOCAL_MACHINE,
the validation layer and the extensions, that requires the admin privilege, by default, this option will be disabled,
if you want to set the vulkan layers location in regedit, just run `cmake --preset <preset_name> -DREGISTER_VULKAN_LAYERS=ON`.
THE PROGRAM WILL NOT WORK if the vulkan layers isn't being set, but the regedit settings only needs to run once, and running it all times is annoying.

## Build static or shared:

By default, the project set the libraries as SHARED, if you wish disable and build the project staticly, just type `cmake --preset <preset_name> [extra_args] -DBUILD_SHARED_LIBS=OFF`

# Running

The first run, the application will start with the Vulkan API, and a file `render.json` will be generated, in the next run,
you can edit the json file and edit the graphics API property, with the following string values:

## Vulkan values:

- `SAMPLE_RENDER_GRAPHICS_API_VK`
- `SAMPLE_RENDER_GRAPHICS_API_VULKAN`

## Direct3D 12 values:

- `SAMPLE_RENDER_GRAPHICS_API_DX12`
- `SAMPLE_RENDER_GRAPHICS_API_D3D12`
- `SAMPLE_RENDER_GRAPHICS_API_DIRECTX12`
- `SAMPLE_RENDER_GRAPHICS_API_DIRECT3D12`
