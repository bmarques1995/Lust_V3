param (
    [string]$moduleDestination
)

if ($moduleDestination -ne "") {
    $url = "https://www.nuget.org/api/v2/package/Microsoft.Direct3D.D3D12/1.614.1"

    # Extract the first two version numbers using a regular expression
    if ($url -match "/(\d+)\.(\d+)\.\d+") {
        $middleVersion = "$($matches[2])"
        $middleVersion | Out-File -FilePath "$moduleDestination/d3d12sdk_version.txt" -Encoding UTF8
    } else {
        Write-Output "No match found"
    }

    Invoke-WebRequest "$url" -OutFile "$moduleDestination/d3d12core.zip"
    Expand-Archive -Path "$moduleDestination/d3d12core.zip" -DestinationPath "$moduleDestination/D3D12Core"
} else {
    Write-Output "Invalid module destination"
}
