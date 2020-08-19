$hexPath = "HandbellController\HandbellController.ino.leonardo.hex"

# $avrPath = "C:\Program Files (x86)\Arduino\hardware\tools\avr"
$avrPath = "HandbellController\avr"
if (-not (Test-Path $avrPath)) {
    Write-Host -ForegroundColor Red  "avrdude.exe path not found"
    exit 1
}

$devices = @((wmic path win32_pnpentity get caption /format:list | select-string "Arduino") | ForEach-Object { $_.Line -replace ".*(COM\d+).*",'$1' })
if ($devices.Length -eq 1) {
    $dev = $devices[0]
    Write-Host "Found Arduino on $dev"
    mode ($dev + ":") "BAUD=1200" "parity=N" "data=8" "stop=1" > $null
    do {
        $newDevices = @((wmic path win32_pnpentity get caption /format:list | select-string "Arduino Leonardo bootloader") | ForEach-Object { $_.Line -replace ".*(COM\d+).*",'$1' })
        Start-Sleep 1
    } while ($newDevices.Length -eq 0)
    $port = $newDevices

    Write-Host "Flashing over $port"
    & "$avrPath\bin\avrdude.exe" -C "$avrPath\etc\avrdude.conf" -p atmega32u4  -c avr109 -P $port -b 57600 -D -U ("flash:w:" + $hexPath + ":i")
    if ($LASTEXITCODE -eq 0) {
        Write-Host -ForegroundColor Green "Arduino flash succeeded"
        exit 0
    } else {
        Write-Host -ForegroundColor Red "Arduino flash failed"
    }
} elseif ($devices.Length -gt 1) {
    Write-Host -ForegroundColor Red  "More than 1 Arduino found"
} else {
    Write-Host -ForegroundColor Red  "Arduino not found"
}
exit 1
