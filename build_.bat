@echo off
REM # # build_.bat

REM --- Local dir setup ---
if not defined PROJECT_ROOT set PROJECT_ROOT=%cd%
set PROJECT_FILE=%PROJECT_ROOT%\Dungeons.uproject
set UE4="%UE4_ENGINE%"

echo ci build script for: '%PROJECT_FILE%' (%UE4%)...
echo .

REM # do cleanup
if exist %PROJECT_ROOT%\Plugins\RuntimeMeshComponent\Binaries\ rd /s /q %PROJECT_ROOT%\Plugins\RuntimeMeshComponent\Binaries\
if exist %PROJECT_ROOT%\Plugins\DungeonsAPIClient\Binaries\ rd /s /q %PROJECT_ROOT%\Plugins\DungeonsAPIClient\Binaries\
if exist %PROJECT_ROOT%\Plugins\LocValidator\Binaries\ rd /s /q %PROJECT_ROOT%\Plugins\LocValidator\Binaries\
if exist %PROJECT_ROOT%\Plugins\PlayfabServices\Binaries\ rd /s /q %PROJECT_ROOT%\Plugins\PlayfabServices\Binaries\
if exist %PROJECT_ROOT%\Plugins\OnlineSubsystemDungeonsPC\Binaries\ rd /s /q %PROJECT_ROOT%\Plugins\OnlineSubsystemDungeonsPC\Binaries\
if exist %PROJECT_ROOT%\Binaries rd /s /q %PROJECT_ROOT%\Binaries

REM if not defined UE4_ENGINE% ... assume/quit
for %%P in (%*) do if /I "%%P" == "-f" (
	call :additional_cleanup
)
if "%1" == "-clean" (
echo cleanup only -- goto finish
goto finish
)

set CONFIGURATION=Development
REM ## adding 'shipping' flag as an option for alpha <- build script will be refactored with pack-code merge
for %%P in (%*) do if /I "%%P" == "-shipping" set CONFIGURATION=Shipping

:setup_output_dir
if defined RELATIVEOUTPUTPATH (
set OUT_PATH=%cd%\%RELATIVEOUTPUTPATH%
) else (
set OUT_PATH=%cd%\LovikaAutomated
)

setlocal EnableDelayedExpansion

for %%P in (%*) do if /I "%%P" == "-pak" (
	REM # setting 'LovikaPacked' default output directory (legacy)
	if not defined RELATIVEOUTPUTPATH set "OUT_PATH=!OUT_PATH:LovikaAutomated=LovikaPacked!"
	goto packagedbuild 
)

REM ## create missing directory
if not exist !OUT_PATH! md !OUT_PATH!

for %%P in (%*) do if /I "%%P" == "-compile" goto compile
for %%P in (%*) do if /I "%%P" == "-uatcompile" goto uatcompile
REM ## fall back to normail 'build and cook':
goto buildandcook

REM ## helper subroutines and program:
:additional_cleanup
echo FORCE clean: also remove intermediate files
if exist %PROJECT_ROOT%\Plugins\RuntimeMeshComponent\Intermediate\ rd /s /q %PROJECT_ROOT%\Plugins\RuntimeMeshComponent\Intermediate\
if exist %PROJECT_ROOT%\Plugins\DungeonsAPIClient\Intermediate\ rd /s /q %PROJECT_ROOT%\Plugins\DungeonsAPIClient\Intermediate\
if exist %PROJECT_ROOT%\Plugins\LocValidator\Intermediate\ rd /s /q %PROJECT_ROOT%\Plugins\LocValidator\Intermediate\
if exist %PROJECT_ROOT%\Plugins\PlayfabServices\Intermediate\ rd /s /q %PROJECT_ROOT%\Plugins\PlayfabServices\Intermediate\
if exist %PROJECT_ROOT%\Plugins\OnlineSubsystemDungeonsPC\Intermediate\ rd /s /q %PROJECT_ROOT%\Plugins\OnlineSubsystemDungeonsPC\Intermediate\
if exist %PROJECT_ROOT%\Intermediate rd /s /q %PROJECT_ROOT%\Intermediate
exit /b 0

:buildandcook
echo "BUILDING (OUT PATH : !OUT_PATH!"

%UE4%\Engine\Build\BatchFiles\RunUAT BuildCookRun ^
-project="%PROJECT_FILE%" ^
-platform=Win64 ^
-clientconfig=%CONFIGURATION% ^
-serverconfig=%CONFIGURATION% ^
-build ^
-cook ^
-stage ^
-stagingdirectory=!OUT_PATH!
REM -map=MainMenu ^

goto finish

:packagedbuild
echo "creating PACKAGED build ... (OUT PATH : !OUT_PATH! )"

%UE4%\Engine\Build\BatchFiles\RunUAT BuildCookRun ^
-project="%PROJECT_FILE%" ^
-platform=Win64 ^
-clientconfig=%CONFIGURATION% ^
-serverconfig=%CONFIGURATION% ^
-nop4 ^
-build ^
-cook ^
-allmaps ^
-stage ^
-pak ^
-archive ^
-archivedirectory=!OUT_PATH!
REM -map=MainMenu ^
goto finish

:uatcompile
echo "COMPILING (Unreal Automation Tool)"
%UE4%\Engine\Build\BatchFiles\RunUAT BuildCookRun ^
-project="%PROJECT_FILE%" ^
-platform=Win64 ^
-clientconfig=%CONFIGURATION% ^
-serverconfig=%CONFIGURATION% ^
-allmaps

goto finish

:compile
echo "COMPILING with Build.bat"
set BUILDCALL=%UE4%\Engine\Build\Batchfiles\Build DungeonsEditor Win64 Development %PROJECT_FILE%
echo (call %BUILDCALL%)
call %BUILDCALL%
goto finish

:finish
set GITHEAD=
set UE4=
set CONFIGURATION=
if !errorlevel! gtr 0 (
	echo ERROR while Building, please review log!
	exit !errorlevel!
)

echo DONE
