param(
    [string]$ShaderFile
)

$filename = Split-Path $ShaderFile -Leaf

if ($filename -match "_CS") {
    $profile = "cs_5_0"
}
elseif ($filename -match "_VS") {
    $profile = "vs_5_0"
}
elseif ($filename -match "_PS") {
    $profile = "ps_5_0"
}
elseif ($filename -match "_GS") {
    $profile = "gs_5_0"
}
else {
    Write-Error "Could not determine shader type from filename: $filename"
    exit 1
}

$outputDir = Join-Path (Split-Path $PSScriptRoot -Parent) "csoFiles"
$outputFile = Join-Path $outputDir ([System.IO.Path]::GetFileNameWithoutExtension($ShaderFile) + ".cso")

Write-Host "Compiling $ShaderFile as $profile"

fxc /T $profile $ShaderFile /Fo $outputFile

if ($LASTEXITCODE -eq 0) {
    Write-Host "Generated $outputFile"
}