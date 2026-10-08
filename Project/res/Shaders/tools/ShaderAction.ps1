param(
    [string]$File
)

$extension = [System.IO.Path]::GetExtension($File)

switch ($extension) {
    ".hlsl" {
        Write-Host "Compiling HLSL..."
        & "${PSScriptRoot}\CompileShader.ps1" $File
    }

    ".cso" {
        Write-Host "Disassembling CSO..."
        & "${PSScriptRoot}\CompileCSO.ps1" $File
    }

    default {
        Write-Error "Unsupported shader file type: $extension"
        exit 1
    }
}