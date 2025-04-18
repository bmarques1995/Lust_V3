param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)

if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    git clone --recursive -b release-1.8.2502 https://github.com/microsoft/DirectXShaderCompiler.git "$moduleDestination/modules/dxc"
    cmake -S "$moduleDestination/modules/dxc" -B "$moduleDestination/dependencies/windows/dxc" -G Ninja -C "$moduleDestination/modules/dxc/cmake/caches/PredefinedParams.cmake" -DCMAKE_BUILD_TYPE="$buildMode" -DENABLE_SPIRV_CODEGEN=ON -DCMAKE_C_COMPILER=cl.exe -DCMAKE_CXX_COMPILER=cl.exe -DCMAKE_INSTALL_PREFIX="$installPrefix"
    cmake --build "$moduleDestination/dependencies/windows/dxc" --config "$buildMode" --target install-dxc-headers
    cmake --build "$moduleDestination/dependencies/windows/dxc" --config "$buildMode" --target install-dxc
    cmake --build "$moduleDestination/dependencies/windows/dxc" --config "$buildMode" --target install-dxcompiler
    New-Item -Path "$installPrefix/lib" -Name "dxc" -ItemType Directory
    $libSrc = Join-Path -Path $moduleDestination "dependencies\windows\dxc\lib\dxcompiler.lib"
    $libDst = Join-Path -Path $installPrefix "lib"
    Copy-Item -Path "$libSrc" -Destination "$LibDst"
    Invoke-WebRequest "https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.8.2502/dxc_2025_02_20.zip" -OutFile "$moduleDestination/dxil.zip"
    Expand-Archive -Path "$moduleDestination/dxil.zip" -DestinationPath "$moduleDestination/dxil"
    $libSrc = Join-Path -Path $moduleDestination "dxil\bin\x64\dxil.dll"
    $libDst = Join-Path -Path $installPrefix "bin"
    Copy-Item -Path "$libSrc" -Destination "$LibDst"
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}
