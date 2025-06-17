param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)
if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    $commitHashRelease= "9ef66bc14edd10dee0de3a545b98578363552f66"
    git clone --recursive https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator.git "$moduleDestination/modules/d3d12ma"
    Set-Location "$moduleDestination/modules/d3d12ma"
    git reset --hard "$commitHashRelease"
    cmake -S "$moduleDestination/modules/d3d12ma" -B "$moduleDestination/dependencies/windows/d3d12ma" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DBUILD_SHARED_LIBS=ON
    cmake --build "$moduleDestination/dependencies/windows/d3d12ma" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}
