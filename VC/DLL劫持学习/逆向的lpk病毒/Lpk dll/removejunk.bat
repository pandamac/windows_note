@echo off
echo ----------------------------------------------------
echo Press any key to delete all files with ending:
echo  *.aps *.ncp *.obj *.pch *.tmp *.sbr *.user *.exe
echo Visual c++/.Net junk 
echo ----------------------------------------------------
pause

del /F /Q /S  *.user *.aps *.idb *.ncp *.obj *.pch *.sbr *.tmp *.bsc *.ilk *.sdf *.ncb *.dep *.ipch *.tlog *.dll *.exp *.html *.exe


pause


