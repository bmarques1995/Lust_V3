param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)
if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    git clone --recursive https://github.com/libjpeg-turbo/libjpeg-turbo.git "$moduleDestination/modules/libjpeg_turbo"
    cmake -S "$moduleDestination/modules/libjpeg_turbo" -B "$moduleDestination/dependencies/windows/libjpeg_turbo" -DCMAKE_INSTALL_PREFIX="$installPrefix"
    cmake --build "$moduleDestination/dependencies/windows/libjpeg_turbo" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}
