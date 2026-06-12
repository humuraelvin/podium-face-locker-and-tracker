param(
    [string]$Port = "COM6",

    [string]$Fqbn = "esp32:esp32:esp32"
)

$ErrorActionPreference = "Stop"
$SketchDir = Join-Path $PSScriptRoot "face_tracker"

function Resolve-ArduinoCli {
    $cmd = Get-Command arduino-cli -ErrorAction SilentlyContinue
    if ($cmd) {
        return $cmd.Source
    }
    $candidates = @(
        "C:\Program Files\Arduino CLI\arduino-cli.exe",
        "$env:LOCALAPPDATA\Programs\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe",
        "$env:LOCALAPPDATA\Arduino CLI\arduino-cli.exe"
    )
    foreach ($path in $candidates) {
        if (Test-Path $path) {
            return $path
        }
    }
    throw "arduino-cli was not found. Install from https://arduino.github.io/arduino-cli/latest/installation/ or add it to PATH."
}

$ArduinoCli = Resolve-ArduinoCli
Write-Host "Using arduino-cli: $ArduinoCli"

$LibDir = Join-Path $env:LOCALAPPDATA "Arduino15\libraries"
$LibArgs = @(
    "--library", (Join-Path $LibDir "PubSubClient"),
    "--library", (Join-Path $LibDir "ESP32Servo")
)

Write-Host "Compiling $SketchDir for $Fqbn"
& $ArduinoCli compile --fqbn $Fqbn @LibArgs $SketchDir

Write-Host "Uploading to $Port"
& $ArduinoCli upload -p $Port --fqbn $Fqbn $SketchDir

Write-Host "Done."
