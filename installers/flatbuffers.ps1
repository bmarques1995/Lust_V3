param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)
if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    $commitHashRelease= "1c514626e83c20fffa8557e75641848e1e15cd5e"
    git clone --recursive https://github.com/google/flatbuffers.git "$moduleDestination/modules/flatbuffers"
    Set-Location "$moduleDestination/modules/flatbuffers"
    git reset --hard "$commitHashRelease"
    cmake -S "$moduleDestination/modules/flatbuffers" -B "$moduleDestination/dependencies/windows/flatbuffers" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DBUILD_SHARED_LIBS=ON
    cmake --build "$moduleDestination/dependencies/windows/flatbuffers" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}