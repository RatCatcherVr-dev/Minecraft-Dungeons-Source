@echo off

REM # # node run level validation test
REM # # NOTE Requires NPM/esprima package # #
if not exist node_modules (
call npm install validator
)

validate_levels