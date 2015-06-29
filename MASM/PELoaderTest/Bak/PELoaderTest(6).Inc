
include windows.inc
include kernel32.inc
include user32.inc
include Comctl32.inc
include shell32.inc
include comdlg32.inc

includelib kernel32.lib
includelib user32.lib
includelib Comctl32.lib
includelib shell32.lib
includelib comdlg32.lib

DlgProc	 PROTO	:HWND,:UINT,:WPARAM,:LPARAM

JudgePE    proto   pMem : LPVOID                     ;判断是否是PE文件 
FixImport  proto   pMem : dword                      ;修复PE文件的输入表    
FixReloc   proto   pMem : dword                      ;修复各个重定位地址       
LoadPE     proto   pMem : dword, pPEFile    : dword  ;加载PE文件到内存
FindOep    proto   pMem : dword                      ;找到PE文件的入口点 
.const

IDD_DIALOG1			equ 101
IDC_BN_FILEOP                   equ 1001
IDC_ED_FILE                     equ 1002
IDC_BN_LOAD                     equ 1003
IDC_BN_TEST                     equ 1004
.data
g_szMsgTxt   db "Sunny 2013",0
g_szMsgTitle db "hint",0

g_szErrorMsg db "Error",0
g_szNotPeMsg db "该文件不是PE格式",0
g_szNoMem    db "内存分配错误",0
g_stOfn      OPENFILENAME  <>
g_szFilter   db "ExE files",0,"*.exe",0,"All files",0,"*.*",0,0
g_szCurDir   db ".",0
g_cFname     db MAX_PATH dup (0)
g_hFile      dd 0
g_dImageSize dd 0  ;PE文件在内存中的映像大小 
g_dFileSize  dd 0  ;PE文件在硬盘上的原始大小
g_hMap       dd 0  ;PE文件的内存映射文件句柄
g_pMap       dd 0  ;PE文件的内存映射文件的地址 
g_pLoadMem   dd 0  ;PE文件的装载地址
;#########################################################################

.data?

hInstance			dd ?

;#########################################################################
