@echo off
setlocal enabledelayedexpansion
set errorFlag=0

IF EXIST %~dp0\testResults.log (
    del %~dp0\testResults.log
)

set lineNum=0
for /f "tokens=*" %%i in ('vstest.console.exe --listtests %~dp0\Bin\x64\Release\InWanderLand_ReleaseEditorUnitTests.dll') do (
	set /a lineNum+=1
	if !lineNum! geq 4 (
		vstest.console.exe --testcasefilter:"Name=%%i&Name^!~Snippet" %~dp0\Bin\x64\Release\InWanderLand_ReleaseEditorUnitTests.dll -- RunConfiguration.TestSessionTimeout=20000 >> %~dp0\testResults.log
		if errorlevel 1 (
			echo something has gone wrong!
			set errorFlag=1
		)
    	)
)

type %~dp0\testResults.log

if %errorFlag% equ 1 (
	exit /b 1
) else (
	exit /b 0
)