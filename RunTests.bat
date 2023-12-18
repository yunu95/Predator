@echo off
setlocal enabledelayedexpansion
set lineNum=0
for /f "tokens=*" %%i in ('vstest.console.exe --listtests .\Bin\x64\Debug\InWanderLand.dll') do (
	set /a lineNum+=1
    if !lineNum! geq 4 (
	vstest.console.exe --tests:%%i .\Bin\x64\Debug\InWanderLand.dll
    )
)
