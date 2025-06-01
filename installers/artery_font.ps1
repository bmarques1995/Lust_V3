param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)
if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    git clone --recursive https://github.com/bmarques1995/artery-font-format.git "$moduleDestination/modules/artery_font"
    cmake -S "$moduleDestination/modules/artery_font" -B "$moduleDestination/dependencies/windows/artery_font" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DARTERY_INSTALL=ON
    cmake --build "$moduleDestination/dependencies/windows/artery_font" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}