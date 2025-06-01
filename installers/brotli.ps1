param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)
if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    $commitHashRelease= "ed738e842d2fbdf2d6459e39267a633c4a9b2f5d"
    git clone --recursive https://github.com/google/brotli.git "$moduleDestination/modules/brotli"
    Set-Location "$moduleDestination/modules/brotli"
    git reset --hard "$commitHashRelease"
    cmake -S "$moduleDestination/modules/brotli" -B "$moduleDestination/dependencies/windows/brotli" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DBUILD_SHARED_LIBS=ON
    cmake --build "$moduleDestination/dependencies/windows/brotli" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}