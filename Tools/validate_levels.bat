@echo off

REM using npm to run tests for console- OR test report output!

set STARTDIR=%cd%
cd %STARTDIR%\validator
call npm install --loglevel=error

if "%1" == "-human" (
call npm run-script test-human --loglevel=error

)
call npm test --loglevel=error

cd %STARTDIR%
set STARTDIR=
echo Done
