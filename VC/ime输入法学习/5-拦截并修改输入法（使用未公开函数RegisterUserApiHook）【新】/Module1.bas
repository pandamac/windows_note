Attribute VB_Name = "Module1"
Public Declare Function RegisterWindowMessage Lib "user32" Alias "RegisterWindowMessageA" (ByVal lpString As String) As Long
Public Declare Function SetWindowLong Lib "user32" Alias "SetWindowLongA" (ByVal hWnd As Long, ByVal nIndex As Long, ByVal dwNewLong As Long) As Long
Public Declare Function CallWindowProc Lib "user32" Alias "CallWindowProcA" (ByVal lpPrevWndFunc As Long, ByVal hWnd As Long, ByVal Msg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Public Declare Function LoadLibrary Lib "kernel32" Alias "LoadLibraryA" (ByVal lpLibFileName As String) As Long
Public Declare Function GetProcAddress Lib "kernel32" (ByVal hModule As Long, ByVal lpProcName As String) As Long
Public Declare Function FreeLibrary Lib "kernel32" (ByVal hLibModule As Long) As Long
Public Declare Function GetVersionEx Lib "kernel32" Alias "GetVersionExA" (lpVersionInformation As OSVERSIONINFO) As Long
Public Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)
Public Declare Function ShellExecute Lib "shell32.dll" Alias "ShellExecuteA" (ByVal hWnd As Long, ByVal lpOperation As String, ByVal lpFile As String, ByVal lpParameters As String, ByVal lpDirectory As String, ByVal nShowCmd As Long) As Long
' ----------------未公开函数--------------------------------------
Public Declare Function UnregisterUserApiHook Lib "user32" () As Long
Public Declare Function RegisterUserApiHookXP Lib "user32" Alias "RegisterUserApiHook" (ByVal hInstance As Long, ByVal fnUserApis As Long) As Long
Public Declare Function RegisterUserApiHook2003 Lib "user32" Alias "RegisterUserApiHook" (pRegInfo As HookAPIRegInfo2003) As Long
' ------------------------DLL导出函数-----------------------------
Public Declare Function DLLstartHOOK Lib "hxwdllwx.dll" (ByVal hWnd As Long) As Long   '初始化输入法HOOK
'Public Declare Function DLLstopHOOK Lib "hxwdllwx.dll" (ByVal a As Long, ByVal b As Long) As Long    '（空函数）
Public Declare Function DLLsetHOOKState Lib "hxwdllwx.dll" (ByVal myState As Boolean) As Long  '打开或关闭钩子
Public Declare Function DLLGetPubString Lib "hxwdllwx.dll" () As String   '获得输入法输入
Public Declare Function DLLSetPubString Lib "hxwdllwx.dll" (ByVal tmpStr As String) As Long   '修改输入法输入
' ----------------------------------------------------------------
Type HookAPIRegInfo2003
    m_size As Long
    m_dllname1 As Long
    m_funname1 As Long
    m_dllname2 As Long
    m_funname2 As Long
End Type

Public Type OSVERSIONINFO
        dwOSVersionInfoSize As Long
        dwMajorVersion As Long
        dwMinorVersion As Long
        dwBuildNumber As Long
        dwPlatformId As Long
        szCSDVersion As String * 128      '  Maintenance string for PSS usage
End Type
' -------------------------------
Public Const GWL_WNDPROC = -4
Public Const SW_HIDE = 0

Public WM_HXWDLLWX_QQBTX As Long  '自定义消息
Public PrevWndProc As Long '保存旧的窗口处理函数地址
Public DLLhwnd As Long, DLLFunDre As Long, OSVERSIONINFOX As OSVERSIONINFO, HookAPIRegInfoX2003 As HookAPIRegInfo2003


Public Function SubWndProc(ByVal hWnd As Long, ByVal Msg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Dim tmpS As String
If Msg = WM_HXWDLLWX_QQBTX Then
'如果收到了输入法拦截消息
    tmpS = DLLGetPubString() '获得输入法输入
    Form1.Text1.Text = Form1.Text1.Text & "拦截到输入法输入：" & tmpS & vbCrLf
    tmpS = tmpS & "（被修改）"
    DLLSetPubString tmpS   '修改输入法输入
End If
SubWndProc = CallWindowProc(PrevWndProc, hWnd, Msg, wParam, lParam)   '将消息传给旧的窗口函数继续处理
End Function

