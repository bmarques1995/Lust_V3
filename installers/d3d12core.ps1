param (
    [string]$moduleDestination
)

$direct3DVersionMajor = "1"
$direct3DVersionMinor = "616"
$direct3DVersionPatch = "1"


if ($moduleDestination -ne "") {
    $url = "https://www.nuget.org/api/v2/package/Microsoft.Direct3D.D3D12/$direct3DVersionMajor.$direct3DVersionMinor.$direct3DVersionPatch"

    # Extract the first two version numbers using a regular expression
    $direct3DVersionMinor  | Out-File -FilePath "$moduleDestination/d3d12sdk_version.txt" -Encoding UTF8
    Invoke-WebRequest "$url" -OutFile "$moduleDestination/d3d12core.zip"
    Expand-Archive -Path "$moduleDestination/d3d12core.zip" -DestinationPath "$moduleDestination/D3D12Core"
} else {
    Write-Output "Invalid module destination"
}
