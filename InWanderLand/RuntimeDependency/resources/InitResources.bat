@echo off
setlocal enabledelayedexpansion

:: Resolve the absolute path for the source directory
pushd %~dp0..\resources
set "sourceDir=%CD%"
popd
set "destinationDir=%~dp0"

:: Iterate over files in the source directory
for /r "%sourceDir%" %%i in (*.*) do (
    set "file=%%~nxi"
    set "filePath=%%i"
    set "relativePath=!filePath:%sourceDir%=!"
    set "targetDir=%destinationDir%!relativePath:%%~nxi=!"

    echo "!sourceDir!"
    echo "!relativePath!"
    echo "!targetDir!"
    if not exist "!targetDir!" mkdir "!targetDir!"
    xcopy "%%i" "!targetDir!" /Y
)

echo Files copied successfully.
pause