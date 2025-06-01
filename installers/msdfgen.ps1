param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)
if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    #$commitHashRelease= "c27de5988d7ecfbc9936ee5f936429e2dbc077b9"
    git clone --recursive https://github.com/bmarques1995/msdfgen.git "$moduleDestination/modules/msdfgen"
    #Set-Location "$moduleDestination/modules/msdf_atlas"
    #git reset --hard "$commitHashRelease"
    cmake -S "$moduleDestination/modules/msdfgen" -B "$moduleDestination/dependencies/windows/msdfgen" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DBUILD_SHARED_LIBS=ON -DMSDFGEN_USE_SKIA=OFF -DMSDFGEN_DYNAMIC_RUNTIME=ON -DMSDFGEN_USE_VCPKG=OFF -DMSDFGEN_INSTALL=ON -DMSDFGEN_BUILD_STANDALONE=OFF
    cmake --build "$moduleDestination/dependencies/windows/msdfgen" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}