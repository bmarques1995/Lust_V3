param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)

Write-Output "$installPrefix"

if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    $commitHashRelease= "b514bdc898e2951020cbdca1304b75f5950d1f59"
    git clone --recursive https://github.com/google/googletest.git "$moduleDestination/modules/gtest"
    Set-Location "$moduleDestination/modules/glm"
    git reset --hard "$commitHashRelease"
    cmake -S "$moduleDestination/modules/gtest" -B "$moduleDestination/dependencies/windows/gtest" -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE="$buildMode" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl
    cmake --build "$moduleDestination/dependencies/windows/gtest" --config "$buildMode" --target install
} 
else 
{
    Write-Output "Invalid build args"
}