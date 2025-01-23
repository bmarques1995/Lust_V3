param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)

if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    git clone --recursive -b "vulkan-sdk-1.4.304" https://github.com/KhronosGroup/SPIRV-Headers.git "$moduleDestination/modules/spirv_headers"
    cmake -S "$moduleDestination/modules/spirv_headers" -B "$moduleDestination/dependencies/windows/spirv_headers" -DCMAKE_INSTALL_PREFIX="$installPrefix"
    cmake --build "$moduleDestination/dependencies/windows/spirv_headers" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}
