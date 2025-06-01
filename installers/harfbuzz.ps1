param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)
if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    $commitHashRelease= "63cdd745ffc77150b702a1800f5f961842db25f8"
    git clone --recursive https://github.com/harfbuzz/harfbuzz.git "$moduleDestination/modules/harfbuzz"
    Set-Location "$moduleDestination/modules/harfbuzz"
    git reset --hard "$commitHashRelease"
    cmake -S "$moduleDestination/modules/harfbuzz" -B "$moduleDestination/dependencies/windows/harfbuzz" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DBUILD_SHARED_LIBS=ON
    cmake --build "$moduleDestination/dependencies/windows/harfbuzz" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}
