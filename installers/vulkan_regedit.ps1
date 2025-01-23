param(
    [string]$ValueName = ""
)

function Test-Administrator {
    $currentUser = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($currentUser)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

if( $ValueName -eq "")
{
    exit
}

if (-not (Test-Administrator)) {
    Write-Host "Requesting administrative privileges..."
    Start-Process powershell.exe -ArgumentList "-ExecutionPolicy Bypass -File `"$PSCommandPath`" -SubKey `"$SubKey`" -ValueName `"$ValueName`" -ValueData `"$ValueData`"" -Verb RunAs
    exit
}

try {
    $SubKey = "SOFTWARE\\Khronos\\Vulkan\\ExplicitLayers"
    $ValueData = 0
    $registryPath = "HKLM:\$SubKey"
    if (-not (Test-Path $registryPath)) {
        New-Item -Path $registryPath -Force | Out-Null
    }
    
    $layers = $ValueName -split '\|'

    foreach ($layer in $layers) {
        Set-ItemProperty -Path $registryPath -Name $layer -Value $ValueData -Type DWord -Force
    }
    
    Write-Host "Registry keys created/updated successfully."
} catch {
    Write-Error "Failed to create/update registry key: $_"
    exit 1
}
