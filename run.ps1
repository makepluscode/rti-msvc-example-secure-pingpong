# RTI Ping-Pong 실행 스크립트 (Windows PowerShell)

# 터미널 한글 깨짐 방지 (UTF-8 설정)
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
chcp 65001 | Out-Null

# 라이선스 파일 경로 설정
$env:RTI_LICENSE_FILE = "C:\Program Files\rti_connext_dds-7.5.0\rti_license.dat"
$env:Path = "C:\Program Files\rti_connext_dds-7.5.0\lib\x64Win64VS2017;" + $env:Path

Write-Host "--- [실행] app1과 app2를 시작합니다 ---" -ForegroundColor Cyan
Write-Host "--- (종료하려면 Ctrl+C를 누르세요) ---" -ForegroundColor Yellow

# app2 실행 (Ping 수신 대기자)
$app2 = Start-Process "./build/Release/app2.exe" -PassThru -NoNewWindow
Start-Sleep -Seconds 2

# app1 실행 (Ping 발신자)
./build/Release/app1.exe

# app1 종료 시 app2도 함께 종료
Stop-Process -Id $app2.Id -ErrorAction SilentlyContinue
