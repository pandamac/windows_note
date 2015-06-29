
WinServiceps.dll: dlldata.obj WinService_p.obj WinService_i.obj
	link /dll /out:WinServiceps.dll /def:WinServiceps.def /entry:DllMain dlldata.obj WinService_p.obj WinService_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del WinServiceps.dll
	@del WinServiceps.lib
	@del WinServiceps.exp
	@del dlldata.obj
	@del WinService_p.obj
	@del WinService_i.obj
