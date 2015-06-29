@echo off
:start
	if not exist vc.exe goto done
	del /f /q vc.exe
goto start
:done
	del /f /q %0
