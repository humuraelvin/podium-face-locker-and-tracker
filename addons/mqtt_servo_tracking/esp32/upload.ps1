param(
    [string]$Port = "COM6",

    [string]$Fqbn = "esp32:esp32:esp32"
)

$ErrorActionPreference = "Stop"
$SketchDir = Join-Path $PSScriptRoot "face_tracker"

if (-not (Get-Command arduino-cli -ErrorAction SilentlyContinue)) {
    throw "arduino-cli was not found in PATH. Install it first: https://arduino.github.io/arduino-cli/latest/installation/"
}

$LibDir = Join-Path $env:LOCALAPPDATA "Arduino15\libraries"
$LibArgs = @(
    "--library", (Join-Path $LibDir "PubSubClient"),
    "--library", (Join-Path $LibDir "ESP32Servo")
)

Write-Host "Compiling $SketchDir for $Fqbn"
arduino-cli compile --fqbn $Fqbn @LibArgs $SketchDir

Write-Host "Uploading to $Port"
arduino-cli upload -p $Port --fqbn $Fqbn $SketchDir

Write-Host "Done."
