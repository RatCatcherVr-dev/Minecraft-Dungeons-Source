@echo off

node %~dp0\validator\cli.js %1
if %errorlevel% neq 0 exit /b %errorlevel%
node %~dp0\validator\common\splicer\cli.js %1