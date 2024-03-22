@echo off

svn add C:\1_Executable/*
svn commit C:\1_Executable -m "auto commit by jenkins"

echo Files commited
pause