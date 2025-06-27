param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)

if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    git clone --recursive -b "vulkan-sdk-1.4.313" https://github.com/KhronosGroup/SPIRV-Tools.git "$moduleDestination/modules/spirv_tools"
    cmake -S "$moduleDestination/modules/spirv_tools" -B "$moduleDestination/dependencies/windows/spirv_tools" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DSPIRV_SKIP_TESTS=ON -DSPIRV-Headers_SOURCE_DIR="$moduleDestination/modules/spirv_headers" -DCMAKE_BUILD_TYPE="$buildMode"
    cmake --build "$moduleDestination/dependencies/windows/spirv_tools" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}
