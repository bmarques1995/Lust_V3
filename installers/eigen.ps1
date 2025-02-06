param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)

if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    $commitHashRelease= "80a43aa9b64030639d1b1bd0c01fae801ab722f537ed"
    git clone --recursive https://gitlab.com/libeigen/eigen.git "$moduleDestination/modules/eigen"
    Set-Location "$moduleDestination/modules/eigen"
    git reset --hard "$commitHashRelease"
    cmake -S "$moduleDestination/modules/eigen" -B "$moduleDestination/dependencies/windows/eigen" -DCMAKE_INSTALL_PREFIX="$installPrefix"
    cmake --build "$moduleDestination/dependencies/windows/eigen" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}
