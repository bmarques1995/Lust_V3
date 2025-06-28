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
    git clone --recursive -b 6.9.1 https://code.qt.io/qt/qtbase.git "$moduleDestination/modules/qtbase"
    cmake -S "$moduleDestination/modules/qtbase" -B "$moduleDestination/dependencies/windows/qtbase" -G "Ninja" -DQT_FEATURE_force_bundled_libs=ON -DCMAKE_PREFIX_PATH="$installPrefix" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DCMAKE_C_COMPILER=cl.exe -DCMAKE_CXX_COMPILER=cl.exe -DCMAKE_BUILD_TYPE="$buildMode" -DCMAKE_ASM_COMPILER=ml64.exe
    cmake --build "$moduleDestination/dependencies/windows/qtbase" --parallel
    cmake --install "$moduleDestination/dependencies/windows/qtbase"
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}
