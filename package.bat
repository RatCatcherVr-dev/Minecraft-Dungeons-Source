@echo off

REM ## make sure is running from project root
if not exist Dungeons.uproject (
    echo "Need to run package.bat from project root!"
    exit /b 1
)
setlocal EnableDelayedExpansion

REM ## early out if running UE4_Editor
setlocal
for %%G in ("UE4Editor.exe", "UE4Editor-Win64-Debug.exe") do (
    call :check_running %%G
    echo erf %errorlevel%
    if %errorlevel% gtr 0 exit /b 1
)
exit /b 0

goto :write_sublevelfiles

:check_running
set DEPENDANT_PROC=%1
echo ..%DEPENDANT_PROC%
tasklist /FI "IMAGENAME eq %DEPENDANT_PROC%" 2>NUL | find /I /N %DEPENDANT_PROC%>NUL
REM ## returns errorlevel=0 if found
if "%ERRORLEVEL%"=="0" (
    echo Needs %DEPENDANT_PROC% unloaded to build! Please close and try again.
    exit /b 1
)
    echo "tasklist erf NEQ 0"
exit /b 0


:write_sublevelfiles
REM ## init sublevels file
echo. > sublevels.txt
echo [/Script/UnrealEd.ProjectPackagingSettings] >> sublevels.txt
REM Optionally list from absolute path for build server?
for /d %%d in ("Content\Decor\Maps\*") do (
    echo do: %%d\SubLevels
    call :add_filenames %%d\SubLevels
)
goto :build_and_package

:add_filenames
set SUBLEVEL_DIR=%1

REM set SUBLEVEL_DIR=Content\Decor\Maps\*\SubLevels
if not exist %SUBLEVEL_DIR% ( 
    echo No SubLevels dir found: '%SUBLEVEL_DIR%'
    exit /b 1
)

for %%a in (%SUBLEVEL_DIR%\*.umap) do (
    REM # batch type string find/replace/edit...
    for /F "tokens=1* delims=." %%f in ("%%a") do (
        set fpath=%%f
        set fpath=!fpath:Content=/Game!
        set fpath=!fpath:\=/!
        echo +MapsToCook=^(FilePath="!fpath!"^)>> sublevels.txt
    )
    REM echo FULL: %%~pnxa
)
exit /B

:build_and_package
endlocal

REM ## add to settings file
set GAMEINI=Config\DefaultGame.ini
copy /Y "%GAMEINI%" "%GAMEINI%.bak"
type sublevels.txt >> %GAMEINI%

REM ## run normal build script...
copy BuildTools\build_.bat .
call build_ -pak


REM ## clean up
copy /Y "%GAMEINI%.bak" "%GAMEINI%"
del %GAMEINI%.bak
del build_.bat
set GAMEINI=
set SUBLEVEL_DIR=
del sublevels.txt
echo Done!

