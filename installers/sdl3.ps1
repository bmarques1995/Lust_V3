param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)
if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    $commitHashRelease= "2fa1e7258a1fd9e3a7a546218b5ed1564953ad39"
    git clone --recursive https://github.com/libsdl-org/SDL.git "$moduleDestination/modules/sdl3"
    Set-Location "$moduleDestination/modules/sdl3"
    git reset --hard "$commitHashRelease"
    cmake -S "$moduleDestination/modules/sdl3" -B "$moduleDestination/dependencies/windows/sdl3" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DBUILD_SHARED_LIBS=ON
    cmake --build "$moduleDestination/dependencies/windows/sdl3" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}