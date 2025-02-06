param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)

if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    $commitHashRelease= "0af55ccecd98d4e5a8d1fad7de25ba429d60e863"
    git clone --recursive https://github.com/g-truc/glm.git "$moduleDestination/modules/glm"
    Set-Location "$moduleDestination/modules/glm"
    git reset --hard "$commitHashRelease"
    cmake -S "$moduleDestination/modules/glm" -B "$moduleDestination/dependencies/windows/glm" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DGLM_BUILD_LIBRARY=OFF -DGLM_BUILD_TESTS=OFF -DGLM_BUILD_INSTALL=ON
    cmake --build "$moduleDestination/dependencies/windows/glm" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}
