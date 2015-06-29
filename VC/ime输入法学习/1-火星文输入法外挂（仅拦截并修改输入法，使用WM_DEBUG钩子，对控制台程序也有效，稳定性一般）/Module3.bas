Attribute VB_Name = "Module3"
Public Declare Sub keybd_event Lib "user32" (ByVal bVk As Byte, ByVal bScan As Byte, ByVal dwFlags As Long, ByVal dwExtraInfo As Long)
Public Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)
Public Declare Function RegisterWindowMessage Lib "user32" Alias "RegisterWindowMessageA" (ByVal lpString As String) As Long
Public Declare Function SetWindowLong Lib "user32" Alias "SetWindowLongA" (ByVal hwnd As Long, ByVal nIndex As Long, ByVal dwNewLong As Long) As Long
Public Declare Function CallWindowProc Lib "user32" Alias "CallWindowProcA" (ByVal lpPrevWndFunc As Long, ByVal hwnd As Long, ByVal Msg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Public Declare Function FindWindowEx Lib "user32" Alias "FindWindowExA" (ByVal Hwnd1 As Long, ByVal Hwnd2 As Long, ByVal lpsz1 As String, ByVal lpsz2 As String) As Long
Public Declare Function SendMessage Lib "user32" Alias "SendMessageA" (ByVal hwnd As Long, ByVal wMsg As Long, ByVal wParam As Long, lParam As Any) As Long
Public Declare Function GetWindowText Lib "user32" Alias "GetWindowTextA" (ByVal hwnd As Long, ByVal lpString As String, ByVal cch As Long) As Long
Public Declare Function ShellExecute Lib "shell32.dll" Alias "ShellExecuteA" (ByVal hwnd As Long, ByVal lpOperation As String, ByVal lpFile As String, ByVal lpParameters As String, ByVal lpDirectory As String, ByVal nShowCmd As Long) As Long
Public Declare Function RegisterHotKey Lib "user32" (ByVal hwnd As Long, ByVal id As Long, ByVal fsModifiers As Long, ByVal vk As Long) As Long
Public Declare Function UnregisterHotKey Lib "user32" (ByVal hwnd As Long, ByVal id As Long) As Long
Public Declare Function Shell_NotifyIcon Lib "shell32.dll" Alias "Shell_NotifyIconA" (ByVal dwMessage As Long, lpData As NOTIFYICONDATA) As Long

Public Declare Function DLLstartHOOK Lib "hxwdllwx.dll" (ByVal hwnd As Long) As Long
Public Declare Function DLLstopHOOK Lib "hxwdllwx.dll" () As Long
Public Declare Function DLLsetHOOKState Lib "hxwdllwx.dll" (ByVal myState As Boolean) As Long
Public Declare Function DLLGetPubString Lib "hxwdllwx.dll" () As String
Public Declare Function DLLSetPubString Lib "hxwdllwx.dll" (ByVal tmpstr As String) As Long

Public Type NOTIFYICONDATA
        cbSize As Long
        hwnd As Long
        uID As Long
        uFlags As Long
        uCallbackMessage As Long
        hIcon As Long
        szTip As String * 64
End Type

Public Const MOD_CONTROL = &H2
Public Const WM_HOTKEY = &H312
Public Const NIM_ADD = 0
Public Const NIM_DELETE = 2
Public Const NIF_MESSAGE = 1
Public Const NIF_ICON = 2
Public Const NIF_TIP = 4

Public Const EM_REPLACESEL = &HC2
Public Const BM_CLICK = &HF5
Public Const WM_GETTEXT = &HD
Public Const WM_SETTEXT = &HC
Public Const WM_LBUTTONDBLCLK = &H203
Public Const WM_RBUTTONUP = &H205

Public Const GWL_WNDPROC = (-4)
Public Const KEYEVENTF_KEYUP = &H2
Public Const VK_Ctrl = &H11
Public Const VK_ALT = &H12
Public Const VK_SHIFT = &H10
Public Const VK_F1 = &H70
Public Const VK_F2 = &H71
Public Const VK_F3 = &H72
Public Const VK_F4 = &H73
Public Const VK_F5 = &H74
Public Const VK_F6 = &H75
Public Const VK_F7 = &H76
Public Const VK_F8 = &H77
Public Const VK_F9 = &H78
Public Const VK_F10 = &H79
Public Const VK_F11 = &H7A
Public Const VK_F12 = &H7B
Public Const VK_ESC = &H1B

Public WM_HXWDLLWX_QQBTX As Long
Public PrevWndProc As Long
Public g_myICONs As NOTIFYICONDATA
Public WM_TRAY_CALLBACK As Long


Public Function SubWndProc(ByVal hwnd As Long, ByVal Msg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Dim myConStr As String, tmpStr1 As String, tmpStr2 As String
If Msg = WM_HXWDLLWX_QQBTX Then
   myConStr = DLLGetPubString()
   tmpStr1 = Replace(myConStr, Chr(0), "")
   If tmpStr1 <> "" Then
      tmpStr2 = pub_fun_jt2bt(tmpStr1)
   Else
      tmpStr2 = ""
   End If
   DLLSetPubString tmpStr2
End If
If Msg = WM_HOTKEY Then
   If wParam = 1 Then
      If Form1.Option1(0).Value = True Then
         Form1.Option1(1).Value = True
      Else
         Form1.Option1(0).Value = True
      End If
   End If
End If
If Msg = WM_TRAY_CALLBACK Then
   If lParam = WM_RBUTTONUP Then
      Form1.PopupMenu Form1.gmypop0
   End If
   If lParam = WM_LBUTTONDBLCLK Then
      g_showMainForm
   End If
End If
SubWndProc = CallWindowProc(PrevWndProc, hwnd, Msg, wParam, lParam)
End Function

' ------------------------------------------------------------------

Sub g_showMainForm()
g_myICONs.uFlags = 0
Shell_NotifyIcon NIM_DELETE, g_myICONs
Form1.Show
End Sub

Sub findQQWriteWind(ByVal qqhwnd As Long, ByRef sendhwnd As Long, ByRef texthwnd As Long, ByRef Listhwnd As Long)
Dim h1 As Long, h2 As Long
h1 = FindWindowEx(qqhwnd, 0, "#32770", vbNullString)
sendhwnd = FindWindowEx(h1, 0, "Button", "发送(S)")
Listhwnd = FindWindowEx(h1, 0, "RichEdit20A", vbNullString)
h2 = FindWindowEx(h1, Listhwnd, "AfxWnd42", vbNullString)
texthwnd = FindWindowEx(h2, 0, vbNullString, vbNullString)
End Sub

Sub findQQWriteWind1(ByVal qqhwnd As Long, ByRef Listhwnd As Long, ByRef Rehwnd As Long)
Dim h1 As Long, h2 As Long
h1 = FindWindowEx(qqhwnd, 0, "#32770", vbNullString)
Listhwnd = FindWindowEx(h1, 0, "RichEdit20A", vbNullString)
Rehwnd = FindWindowEx(h1, 0, "Button", "回讯息(&R)")
End Sub

Sub findQQWriteWind2(ByVal qqhwnd As Long, ByRef texthwnd As Long, ByRef sendhwnd As Long)
Dim h1 As Long, h2 As Long
h1 = FindWindowEx(qqhwnd, 0, "#32770", vbNullString)
sendhwnd = FindWindowEx(h1, 0, "Button", "发送(S)")
texthwnd = FindWindowEx(h1, sendhwnd, "AfxWnd42", vbNullString)
texthwnd = FindWindowEx(texthwnd, 0, "RichEdit20A", vbNullString)
End Sub

Function findQQgetText(ByVal fhwnd As Long) As String
Dim temp_s As String
temp_s = String(1024, Chr(0))
SendMessage fhwnd, WM_GETTEXT, Len(temp_s), ByVal temp_s
temp_s = Left(temp_s, InStr(temp_s, Chr(0)) - 1)
findQQgetText = temp_s
End Function

Sub findQQsetText(ByVal fhwnd As Long, ByVal mystr As String)
SendMessage fhwnd, WM_SETTEXT, 0&, ByVal mystr
End Sub

Sub setQQText(ByVal fhwnd As Long, ByVal mystr As String)
'Dim mydata() As Byte, i As Long, tmp_k As Long
'i = 0
'mydata = StrConv(mystr, vbFromUnicode)
'tmp_k = UBound(mydata)
'While i <= tmp_k
'      If mydata(i) < 128 Then
'          PostMessage fhwnd, WM_CHAR, mydata(i), 0&
'          i = i + 1
'      Else
'          PostMessage fhwnd, WM_CHAR, mydata(i), 0&
'          PostMessage fhwnd, WM_CHAR, mydata(i + 1), 0&
'          i = i + 2
'      End If
'Wend
SendMessage fhwnd, EM_REPLACESEL, 0, ByVal mystr
End Sub

Sub myClickBotton(ByVal fhwnd As Long)
'按下某个按钮
'PostMessage fhwnd, BM_CLICK, 0&, 0&
SendMessage fhwnd, BM_CLICK, 0, ByVal 0
End Sub

Function myGetQQTextToStr(ByVal tmpstr As String) As String
Dim mytmpstr
If Len(tmpstr) > 20 Then
   If Left(tmpstr, 2) = Chr(32) & Chr(32) Then
      tmpstr = Right(tmpstr, Len(tmpstr) - InStr(tmpstr, Chr(10)))
   End If
End If

If Len(tmpstr) >= 3 Then
tmpstr = Right(tmpstr, Len(tmpstr) - 2)
End If

mytmpstr = Split(tmpstr, Chr(13) & Chr(10))
tmpstr = ""
If UBound(mytmpstr) >= 1 Then
   For i = 1 To UBound(mytmpstr)
   tmpstr = tmpstr & mytmpstr(i)
   Next
End If
tmpstr = Replace(tmpstr, Chr(10), " ")
tmpstr = Replace(tmpstr, Chr(13), " ")
myGetQQTextToStr = tmpstr
End Function

Sub QQMsgDoLoop()
Dim qqhwnd As Long, mystr As String * 255
qqhwnd = 0
Do
   qqhwnd = FindWindowEx(0, qqhwnd, "#32770", vbNullString)
   If qqhwnd <> 0 Then
      GetWindowText qqhwnd, mystr, Len(mystr) - 1
      If (mystr Like "与*聊天中*") Or (mystr Like "*正在输入*") Or (mystr Like "与*交谈中*") Then
          QQchatListMode qqhwnd
      Else
          If (mystr Like "*查看消息*") Then
             QQchatMessageMode qqhwnd
          End If
      End If
   End If
Loop Until qqhwnd = 0
End Sub

Sub QQchatListMode(ByVal qqhwnd As Long)
'聊天模式
Dim h1 As Long, h2 As Long, h3 As Long, myStrT As String
findQQWriteWind qqhwnd, h1, h2, h3
myStrT = findQQgetText(h3)
If myStrT = "" Then Exit Sub
myStrT = myGetQQTextToStr(myStrT)
If myStrT = "" Then
   findQQsetText h3, ""
   Exit Sub
End If
Form1.Text11.Text = myStrT
qqchat_kernel
myStrT = Form1.Text10.Text
If myStrT = "" Then myStrT = "..."
setQQText h2, myStrT
Sleep 50
myClickBotton h1
Sleep 300
findQQsetText h3, ""
End Sub

Sub QQchatMessageMode(ByVal qqhwnd As Long)
'消息模式
Dim h1 As Long, h2 As Long, myStrT As String
findQQWriteWind1 qqhwnd, h1, h2
myStrT = findQQgetText(h1)
myClickBotton h2
If myStrT <> "" Then myStrT = myGetQQTextToStr(myStrT)
Form1.Text11.Text = myStrT
qqchat_kernel
myStrT = Form1.Text10.Text
If myStrT = "" Then myStrT = "..."
Sleep 150
qqhwnd = getQQreHwnd()
If qqhwnd = 0 Then
myClickBotton h2
Sleep 150
qqhwnd = getQQreHwnd()
End If
If qqhwnd = 0 Then
myClickBotton h2
Sleep 300
qqhwnd = getQQreHwnd()
End If
If qqhwnd = 0 Then
myClickBotton h2
Sleep 300
qqhwnd = getQQreHwnd()
End If
If qqhwnd = 0 Then
Sleep 300
qqhwnd = getQQreHwnd()
End If
If qqhwnd = 0 Then
Sleep 300
qqhwnd = getQQreHwnd()
End If
If qqhwnd = 0 Then
Sleep 400
qqhwnd = getQQreHwnd()
End If
If qqhwnd = 0 Then Exit Sub
findQQWriteWind2 qqhwnd, h1, h2
setQQText h1, myStrT
Sleep 50
myClickBotton h2
End Sub

Function getQQreHwnd() As Long
Dim qqhwnd As Long, mystr As String * 255
qqhwnd = 0
Do
   qqhwnd = FindWindowEx(0, qqhwnd, "#32770", vbNullString)
   If qqhwnd <> 0 Then
      GetWindowText qqhwnd, mystr, Len(mystr) - 1
      If (mystr Like "*发送消息*") Then
         getQQreHwnd = qqhwnd
         Exit Function
      End If
   End If
Loop Until qqhwnd = 0
getQQreHwnd = 0
End Function





