param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)

if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    git clone --recursive -b "vulkan-sdk-1.4.313" https://github.com/KhronosGroup/Vulkan-ValidationLayers.git "$moduleDestination/modules/vulkan_validation"
    cmake -S "$moduleDestination/modules/vulkan_validation" -B "$moduleDestination/dependencies/windows/vulkan_validation" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DUPDATE_DEPS=ON -DCMAKE_BUILD_TYPE="Release"
    cmake --build "$moduleDestination/dependencies/windows/vulkan_validation" --config Release --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}
