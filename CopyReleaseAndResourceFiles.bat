@echo off
setlocal enabledelayedexpansion

set "sourceDir=%~dp0Bin\x64\Release"
set "baseDestinationDir=C:\1_Executable"
set "filterFile=%~dp0CIFilter.txt"

:: Create a timestamped destination directory
:: set "timestamp=%date:~10,4%-%date:~4,2%-%date:~7,2%-%time:~0,2%-%time:~3,2%"
:: Get a consistent timestamp using WMIC
for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "timestamp=%dt:~0,4%-%dt:~4,2%-%dt:~6,2%-%dt:~8,2%-%dt:~10,2%"
set "destinationDir=%baseDestinationDir%\%timestamp%"
set "destinationDirAlt=%baseDestinationDir%\resources"

:: Create an associative array (kind of) for filenames and extensions
set "index=0"
for /f "tokens=*" %%a in (%filterFile%) do (
    set /a index+=1
    set "filter[!index!]=%%a"
)

:: Iterate over files in the source directory
for /r "%sourceDir%" %%i in (*.*) do (
    set "file=%%~nxi"
    set "copyFile=false"

    :: Check if the file matches any of the filters
    for /l %%x in (1,1,!index!) do (
        set "currentFilter=!filter[%%x]!"

        if /i "!currentFilter!"=="*.*" (
            set "copyFile=true"
        ) else if /i "!currentFilter!"=="!file!" (
            set "copyFile=true"
        ) else if /i "!currentFilter!"=="%%~xi" (
            set "copyFile=true"
        )
    )

    if !copyFile! equ true (
        set "filePath=%%i"
        set "relativePath=!filePath:%sourceDir%=!"
        set "targetDir=%destinationDir%!relativePath:%%~nxi=!"

        echo "!relativePath!"
        echo "!targetDir!"
        if not exist "!targetDir!" mkdir "!targetDir!"
        xcopy "%%i" "!targetDir!" /Y
    ) else (
        set "filePath=%%i"
        set "relativePath=!filePath:%sourceDir%=!"
        set "targetDir=%destinationDirAlt%!relativePath:%%~nxi=!"

        echo "!relativePath!"
        echo "!targetDir!"
        if not exist "!targetDir!" mkdir "!targetDir!"
        xcopy "%%i" "!targetDir!" /D /E /C /Q /H /R /Y /K /I /S
    )
)

echo Files copied successfully.
pause