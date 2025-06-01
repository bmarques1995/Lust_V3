param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)
if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    $commitHashRelease= "42608f77f20749dd6ddc9e0536788eaad70ea4b5"
    git clone --recursive https://gitlab.freedesktop.org/freetype/freetype.git "$moduleDestination/modules/freetype"
    Set-Location "$moduleDestination/modules/freetype"
    git reset --hard "$commitHashRelease"
    cmake -S "$moduleDestination/modules/freetype" -B "$moduleDestination/dependencies/windows/freetype" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DBUILD_SHARED_LIBS=ON -DFT_DISABLE_BZIP2=ON
    cmake --build "$moduleDestination/dependencies/windows/freetype" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}