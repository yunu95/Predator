@echo off

svn cleanup C:\1_Executable\autoRelease
svn update C:\1_Executable\autoRelease
svn resolve C:\1_Executable\autoRelease/* --accept mine-full
svn add C:\1_Executable\autoRelease/*
svn commit C:\1_Executable\autoRelease -m "auto commit by jenkins"

echo Files commited