param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)
if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    $commitHashRelease= "c27de5988d7ecfbc9936ee5f936429e2dbc077b9"
    git clone https://github.com/Chlumsky/msdf-atlas-gen.git "$moduleDestination/modules/msdf_atlas"
    Set-Location "$moduleDestination/modules/msdf_atlas"
    git reset --hard "$commitHashRelease"
    git submodule update --init ./artery-font-format
    cmake -S "$moduleDestination/modules/msdf_atlas" -B "$moduleDestination/dependencies/windows/msdf_atlas" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DMSDF_ATLAS_MSDFGEN_EXTERNAL=ON -DBUILD_SHARED_LIBS=ON -DMSDF_ATLAS_USE_SKIA=OFF -DMSDF_ATLAS_DYNAMIC_RUNTIME=ON -DMSDF_ATLAS_USE_VCPKG=OFF -DMSDF_ATLAS_INSTALL=ON -DMSDF_ATLAS_BUILD_STANDALONE=OFF
    cmake --build "$moduleDestination/dependencies/windows/msdf_atlas" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}