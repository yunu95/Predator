@echo off
setlocal enabledelayedexpansion
set lineNum=0
set errorFlag=0

for /f "tokens=*" %%i in ('vstest.console.exe --listtests %~dp0\Bin\x64\Debug\InWanderLand.dll') do (
	set /a lineNum+=1
	if !lineNum! geq 4 (
		vstest.console.exe --tests:%%i %~dp0\Bin\x64\Debug\InWanderLand.dll >> testResults.log
		if errorlevel 1 (
			set errorFlag=1
		)
    	)
)

if %errorFlag% equ 1 (
	exit /b 1
) else (
	exit /b 0
)