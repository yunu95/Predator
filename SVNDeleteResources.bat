@echo off

svn cleanup C:\1_Executable\autoRelease
svn update C:\1_Executable\autoRelease
svn resolve C:\1_Executable\autoRelease/* --accept mine-full
svn delete C:\1_Executable\autoRelease/resources --force

echo Files commited