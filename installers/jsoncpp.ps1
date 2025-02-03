param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)
if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    $commitHashRelease= "89e2973c754a9c02a49974d839779b151e95afd6"
    git clone --recursive https://github.com/open-source-parsers/jsoncpp.git "$moduleDestination/modules/jsoncpp"
    Set-Location "$moduleDestination/modules/jsoncpp"
    git reset --hard "$commitHashRelease"
    cmake -S "$moduleDestination/modules/jsoncpp" -B "$moduleDestination/dependencies/windows/jsoncpp" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=OFF -DBUILD_OBJECT_LIBS=OFF
    cmake --build "$moduleDestination/dependencies/windows/jsoncpp" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}
