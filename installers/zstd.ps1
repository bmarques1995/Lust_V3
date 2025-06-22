param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)
if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    $commitHashRelease= "f8745da6ff1ad1e7bab384bd1f9d742439278e99"
    git clone --recursive -b release https://github.com/facebook/zstd.git "$moduleDestination/modules/zstd"
    Set-Location "$moduleDestination/modules/zstd"
    git reset --hard "$commitHashRelease"
    cmake -S "$moduleDestination/modules/zstd/build/cmake" -B "$moduleDestination/dependencies/windows/zstd" -DCMAKE_INSTALL_PREFIX="$installPrefix"
    cmake --build "$moduleDestination/dependencies/windows/zstd" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}
