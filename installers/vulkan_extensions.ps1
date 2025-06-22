param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)

if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    git clone --recursive -b "vulkan-sdk-1.4.313" https://github.com/KhronosGroup/Vulkan-ExtensionLayer.git "$moduleDestination/modules/vulkan_extension"
    cmake -S "$moduleDestination/modules/vulkan_extension" -B "$moduleDestination/dependencies/windows/vulkan_extension" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DUPDATE_DEPS=ON -DCMAKE_BUILD_TYPE="$buildMode"
    cmake --build "$moduleDestination/dependencies/windows/vulkan_extension" --config "$buildMode" --target install
}
else
{
    Write-Output $buildMode
    Write-Output $installPrefix
    Write-Output $moduleDestination
    Write-Output "VkExtensions Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}
