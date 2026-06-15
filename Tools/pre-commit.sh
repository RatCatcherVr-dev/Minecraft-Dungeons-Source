#!/bin/bash
echo Running pre-commit tests...
# pwd
# git diff-tree --no-commit-id --name-only -r HEAD
# todo: list of files changes for which to run lint:
FILES_PATTERN='(Content\/data\/lovika|Tools\/validator).+'
ContentFiles=`git diff --name-only --cached | grep -E $FILES_PATTERN | wc -l `
echo Trying to commit $ContentFiles files that REQUIRE level testing...
if [ $ContentFiles -ge 1 ]; then
    exec npm run test-human --prefix Tools/validatorgit st

fi
