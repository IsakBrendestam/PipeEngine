param(
    [string]$CsoFile
)

$directory = Split-Path $CsoFile -Parent
$asmDirectory = Join-Path $directory "asmFiles"

# Create asmFiles folder if it doesn't exist
if (!(Test-Path $asmDirectory)) {
    New-Item -ItemType Directory -Path $asmDirectory | Out-Null
}

$fileName = [System.IO.Path]::GetFileNameWithoutExtension($CsoFile)
$asm = Join-Path $asmDirectory "$fileName.asm"

fxc /dumpbin /Fc $asm $CsoFile