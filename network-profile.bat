@echo off

echo Starting Dungeons ...

".\Dungeons.exe" networkprofiler=true

echo Dungeons exited with error code %ERRORLEVEL%.
echo   - Profile artefacts saved to location
echo       %~dp0Saved\Profiling
echo   - Logs saved to location
echo       %~dp0Saved\Logs

pause