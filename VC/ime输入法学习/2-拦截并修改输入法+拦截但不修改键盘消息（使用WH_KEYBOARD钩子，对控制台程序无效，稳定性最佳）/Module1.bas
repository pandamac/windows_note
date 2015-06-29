Attribute VB_Name = "Module1"
Public Declare Function RegisterWindowMessage Lib "user32" Alias "RegisterWindowMessageA" (ByVal lpString As String) As Long
Public Declare Function SetWindowLong Lib "user32" Alias "SetWindowLongA" (ByVal hwnd As Long, ByVal nIndex As Long, ByVal dwNewLong As Long) As Long
Public Declare Function CallWindowProc Lib "user32" Alias "CallWindowProcA" (ByVal lpPrevWndFunc As Long, ByVal hwnd As Long, ByVal Msg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
' ------------------------DLL导出函数-----------------------------
Public Declare Function DLLstartHOOK Lib "hxwdllwx.dll" (ByVal hwnd As Long) As Long   '初始化钩子
Public Declare Function DLLstopHOOK Lib "hxwdllwx.dll" () As Long   '卸载钩子
Public Declare Function DLLsetHOOKState Lib "hxwdllwx.dll" (ByVal myState As Boolean) As Long  '打开或关闭钩子
Public Declare Function DLLGetPubString Lib "hxwdllwx.dll" () As String   '获得输入法输入
Public Declare Function DLLSetPubString Lib "hxwdllwx.dll" (ByVal tmpstr As String) As Long   '修改输入法输入
' ----------------------------------------------------------------
Public Const GWL_WNDPROC = -4

Public WM_HXWDLLWX_QQBTX As Long  '自定义消息
Public WM_HXWDLLWX_HOOKKEY As Long
Public PrevWndProc As Long '保存旧的窗口处理函数地址


Public Function SubWndProc(ByVal hwnd As Long, ByVal Msg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Dim tmpS As String
If Msg = WM_HXWDLLWX_QQBTX Then
'如果收到了输入法拦截消息
    tmpS = DLLGetPubString() '获得输入法输入
    Form1.Text1.Text = Form1.Text1.Text & "拦截到输入法输入：" & tmpS & vbCrLf
    tmpS = tmpS & "（被修改）"
    DLLSetPubString tmpS   '修改输入法输入
End If
If Msg = WM_HXWDLLWX_HOOKKEY Then
'如果收到的是键盘拦截消息
    Form1.Text1.Text = Form1.Text1.Text & "拦截到键盘消息。扫描码：" & lParam & " 字符：" & Chr(wParam) & vbCrLf
End If
SubWndProc = CallWindowProc(PrevWndProc, hwnd, Msg, wParam, lParam)   '将消息传给旧的窗口函数继续处理
End Function
