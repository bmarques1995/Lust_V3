param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)
if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    $commitHashRelease= "f355b3d58f7067eee1706ff3c801c2361011f3d5"
    git clone --recursive https://github.com/gabime/spdlog.git "$moduleDestination/modules/spdlog"
    Set-Location "$moduleDestination/modules/spdlog"
    git reset --hard "$commitHashRelease"
    cmake -S "$moduleDestination/modules/spdlog" -B "$moduleDestination/dependencies/windows/spdlog" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DBUILD_SHARED_LIBS=ON
    cmake --build "$moduleDestination/dependencies/windows/spdlog" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}