@echo off
cd /d %~dp0\Bin\x64\Release
start %~dp0\Bin\x64\Release\InWanderLand_ReleaseEditorExe.exe
timeout /t 10
tasklist /fi "imagename eq InWanderLand_ReleaseEditorExe.exe"
if errorlevel 1 (
    echo 릴리즈 버전 에디터 프로그램이 동작하지 않습니다. 로컬 컴퓨터에서 Bin 폴더를 삭제하고 릴리즈 버전으로 재빌드후 실행해도 원활히 동작하는지 확인하십시오.
    exit /b 1
) else (
    echo 릴리즈 버전 에디터 프로그램이 잘 동작합니다.
    taskkill /im InWanderLand_ReleaseEditorExe.exe /f
    exit /b 0
)