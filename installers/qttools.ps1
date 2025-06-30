param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination,
    [string]$vsCompilerLocation
)

function AppendVSNinjaHost {
    param (
        [string]$compilerLocation
    )

    $vsVersionRegex = "(.*Community|Enterprise|Professional)"
    $vsBasePath = ""

    if ($compilerLocation -match $vsVersionRegex) {
        $vsBasePath = $matches[1]
    } else {
        Write-Output "Version not found"
    }

    $env:PATH = "$vsBasePath/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja;$env:PATH"
}

if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne "") -and ($vsCompilerLocation -ne ""))
{
    AppendVSNinjaHost($vsCompilerLocation)
    Write-Output "Installing QtSVG with build mode: $buildMode, install prefix: $installPrefix, module destination: $moduleDestination"
    git clone --recursive -b 6.9.1 https://code.qt.io/qt/qttools.git "$moduleDestination/modules/qttools"
    cmake -S "$moduleDestination/modules/qttools" -B "$moduleDestination/dependencies/windows/qttools" -G Ninja -DQT_FEATURE_force_bundled_libs=ON -DCMAKE_INSTALL_PREFIX="$installPrefix" -DCMAKE_PREFIX_PATH="$installPrefix" -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl -DCMAKE_ASM_COMPILER=ml64 -DCMAKE_BUILD_TYPE="$buildMode"
    cmake --build "$moduleDestination/dependencies/windows/qttools" --parallel
    cmake --install "$moduleDestination/dependencies/windows/qttools"
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}
