@echo off
setlocal

REM Change the path to the target directory if needed, or leave as "." for the current directory
set "target_directory=."

REM Recursively find and delete all .meta files
for /r "%target_directory%" %%f in (*.meta) do (
    echo Deleting %%f
    del "%%f"
)

endlocal
