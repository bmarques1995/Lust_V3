param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)
if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    $commitHashRelease= "9148bdf719e997d1f474be6bcc7943881046dba1"
    git clone --recursive https://github.com/leethomason/tinyxml2.git "$moduleDestination/modules/tinyxml2"
    Set-Location "$moduleDestination/modules/tinyxml2"
    git reset --hard "$commitHashRelease"
    cmake -S "$moduleDestination/modules/tinyxml2" -B "$moduleDestination/dependencies/windows/tinyxml2" -DCMAKE_INSTALL_PREFIX="$installPrefix" -Dtinyxml2_SHARED_LIBS=ON 
    cmake --build "$moduleDestination/dependencies/windows/tinyxml2" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}
