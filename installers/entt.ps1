param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)
if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    $commitHashRelease= "d4014c74dc3793aba95ae354d6e23a026c2796db"
    git clone --recursive https://github.com/skypjack/entt.git "$moduleDestination/modules/entt"
    Set-Location "$moduleDestination/modules/entt"
    git reset --hard "$commitHashRelease"
    cmake -S "$moduleDestination/modules/entt" -B "$moduleDestination/dependencies/windows/entt" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DBUILD_SHARED_LIBS=ON -DENTT_INSTALL=ON
    cmake --build "$moduleDestination/dependencies/windows/entt" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}
