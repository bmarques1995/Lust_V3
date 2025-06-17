param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)
if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    $commitHashRelease= "1d8f600fd424278486eade7ed3e877c99f0846b1"
    git clone --recursive https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git "$moduleDestination/modules/vma"
    Set-Location "$moduleDestination/modules/vma"
    git reset --hard "$commitHashRelease"
    cmake -S "$moduleDestination/modules/vma" -B "$moduleDestination/dependencies/windows/vma" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DBUILD_SHARED_LIBS=ON
    cmake --build "$moduleDestination/dependencies/windows/vma" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}
