Attribute VB_Name = "Module1"
Public Declare Function RegisterWindowMessage Lib "user32" Alias "RegisterWindowMessageA" (ByVal lpString As String) As Long
Public Declare Function SetWindowLong Lib "user32" Alias "SetWindowLongA" (ByVal hwnd As Long, ByVal nIndex As Long, ByVal dwNewLong As Long) As Long
Public Declare Function CallWindowProc Lib "user32" Alias "CallWindowProcA" (ByVal lpPrevWndFunc As Long, ByVal hwnd As Long, ByVal Msg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Public Declare Sub CopyMemory Lib "kernel32" Alias "RtlMoveMemory" (Destination As Any, Source As Any, ByVal Length As Long)
' ------------------------DLL导出函数-----------------------------
Public Declare Function DLLstartHOOK Lib "hxwdllwx.dll" (ByVal hwnd As Long) As Long   '初始化钩子
Public Declare Function DLLstopHOOK Lib "hxwdllwx.dll" () As Long   '卸载钩子
Public Declare Function DLLsetHOOKState Lib "hxwdllwx.dll" (ByVal myState As Boolean) As Long  '打开或关闭钩子
Public Declare Function DLLGetPubString Lib "hxwdllwx.dll" () As String   '获得输入法输入
Public Declare Function DLLSetPubString Lib "hxwdllwx.dll" (ByVal tmpstr As String) As Long   '修改输入法输入
Public Declare Function DLLGetPubMsg Lib "hxwdllwx.dll" () As Long   '获得拦截到的键盘消息,返回一个lpMSG类型的指针
' ----------------------------------------------------------------
Public Type POINTAPI
        x As Long
        y As Long
End Type

Public Type lpMSG
' 声明windows消息类型
  hwnd As Long
  message As Long
  wParam As Long
  lParam As Long
  time As Long
  pt As POINTAPI
End Type


Public Const GWL_WNDPROC = -4
Public Const WM_KEYDOWN = &H100
Public Const WM_CHAR = &H102

Public WM_HXWDLLWX_QQBTX As Long  '自定义消息
Public WM_HXWDLLWX_HOOKKEY As Long
Public PrevWndProc As Long '保存旧的窗口处理函数地址


Public Function SubWndProc(ByVal hwnd As Long, ByVal Msg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Dim tmpS As String, myMSG As lpMSG, MSGPoint As Long
If Msg = WM_HXWDLLWX_QQBTX Then
'如果收到了输入法拦截消息
    tmpS = DLLGetPubString() '获得输入法输入
    Form1.Text1.Text = Form1.Text1.Text & "拦截到输入法输入：" & tmpS & vbCrLf
    tmpS = tmpS & "（被修改）"
    DLLSetPubString tmpS   '修改输入法输入
End If
If Msg = WM_HXWDLLWX_HOOKKEY Then
'如果收到的是键盘拦截消息
    MSGPoint = DLLGetPubMsg()
    CopyMemory myMSG, ByVal MSGPoint, Len(myMSG) '将指针MSGPoint所指的内存区域复制到myMSG结构中
    If myMSG.message = WM_KEYDOWN Then
        Form1.Text1.Text = Form1.Text1.Text & "拦截到键盘按下消息。扫描码：" & myMSG.lParam & " 按键：" & Chr(myMSG.wParam) & vbCrLf
        '以下示例了如何修改按键消息。你可以修改消息的任何内容，甚至修改myMSG.message
        If myMSG.wParam = Asc("B") Then myMSG.wParam = Asc("A") '修改按键消息，将按键B改为按下按键“A”
    End If
    If myMSG.message = WM_CHAR Then
        Form1.Text1.Text = Form1.Text1.Text & "拦截到英文字符消息。扫描码：" & myMSG.lParam & " 字符：" & Chr(myMSG.wParam) & vbCrLf
    End If
    CopyMemory ByVal MSGPoint, myMSG, Len(myMSG)  '将myMSG的数据复制回MSGPoint所指的内存区域
End If
SubWndProc = CallWindowProc(PrevWndProc, hwnd, Msg, wParam, lParam)   '将消息传给旧的窗口函数继续处理
End Function
