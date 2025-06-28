param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)

if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    $commitHashRelease= "fbd8b99c2b828428947d70fdc046bb55609be93e"
    git clone --recursive https://github.com/microsoft/mimalloc.git "$moduleDestination/modules/mimalloc"
    Set-Location "$moduleDestination/modules/mimalloc"
    git reset --hard "$commitHashRelease"
    cmake -S "$moduleDestination/modules/mimalloc" -B "$moduleDestination/dependencies/windows/mimalloc" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DCMAKE_PREFIX_PATH="$installPrefix" -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=OFF
    cmake --build "$moduleDestination/dependencies/windows/mimalloc" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}
