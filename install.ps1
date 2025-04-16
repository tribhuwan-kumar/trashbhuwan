Write-Host "installing trasbhuwan..."

try {
    # get latest version
    $releases = Invoke-RestMethod "https://api.github.com/repos/tribhuwan-kumar/trashbhuwan/releases"
    $latestRelease = $releases | Where-Object { -not $_.prerelease } | Select-Object -First 1
    if (-not $latestRelease) {
        throw "no releases found"
    }

    # find the Windows asset
    $asset = $latestRelease.assets | Where-Object { $_.name -like "trashbhuwan-windows.zip" } | Select-Object -First 1
    if (-not $asset) {
        throw "no Windows release found in version $($latestRelease.tag_name)"
    }

    $url = $asset.browser_download_url

    $installDir = "$env:USERPROFILE\.trasbhuwan"
    $tempZip = "$env:TEMP\trashbhuwan-windows.zip"

    # download and extract
    Write-Host "downloading latest version ($($latestRelease.tag_name)) from $url..."
    Invoke-WebRequest -Uri $url -OutFile $tempZip

    # create install directory if it doesn't exist
    if (!(Test-Path $installDir)) {
        New-Item -ItemType Directory -Path $installDir | Out-Null
    }

    Expand-Archive -Path $tempZip -DestinationPath $installDir -Force

    # add to PATH if not already there
    $currentPath = [Environment]::GetEnvironmentVariable("Path", "User")
    if ($currentPath -notlike "*$installDir\*") {
        [Environment]::SetEnvironmentVariable("Path", "$currentPath;$installDir\", "User")
        $env:Path = [Environment]::GetEnvironmentVariable("Path", "User")
    }

    # verify installation
    $binPath = Join-Path $installDir "trashbhuwan.exe"
    if (!(Test-Path $binPath)) {
        throw "trasbhuwan.exe not found in $binPath after installation"
    }

    # cleanup
    Remove-Item $tempZip -Force
    Write-Host "trasbhuwan installed successfully, restart the terminal and run command 'trashbhuwan'"
} catch {
    Write-Host "installation failed: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}
