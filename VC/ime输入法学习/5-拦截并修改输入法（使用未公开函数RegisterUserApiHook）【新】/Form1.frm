VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   3885
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   6795
   LinkTopic       =   "Form1"
   ScaleHeight     =   3885
   ScaleWidth      =   6795
   StartUpPosition =   3  '窗口缺省
   Begin VB.TextBox Text1 
      Height          =   2895
      Left            =   240
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   2
      Top             =   240
      Width           =   6255
   End
   Begin VB.CommandButton Command2 
      Caption         =   "关闭拦截"
      Height          =   375
      Left            =   3960
      TabIndex        =   1
      Top             =   3360
      Width           =   1455
   End
   Begin VB.CommandButton Command1 
      Caption         =   "打开拦截"
      Height          =   375
      Left            =   960
      TabIndex        =   0
      Top             =   3360
      Width           =   1455
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Command1_Click()
DLLsetHOOKState True
End Sub

Private Sub Command2_Click()
DLLsetHOOKState False
End Sub

Private Sub Form_Load()
OSVERSIONINFOX.dwOSVersionInfoSize = Len(OSVERSIONINFOX)
GetVersionEx OSVERSIONINFOX  '取得操作系统版本
DLLhwnd = LoadLibrary("hxwdllwx.dll")   '加载DLL
DLLFunDre = GetProcAddress(DLLhwnd, "DLLstopHOOK")    '获取回调函数地址

WM_HXWDLLWX_QQBTX = RegisterWindowMessage("WM_HXWDLLWX_QQBTX")  '注册自定义消息
PrevWndProc = SetWindowLong(Me.hWnd, GWL_WNDPROC, AddressOf SubWndProc) '子类化窗口，以便能处理DLL发出的自定义消息
' ------------------------
Dim retV As Long, tmpStr As String
tmpStr = App.Path
If Right(tmpStr, 1) <> "\" Then tmpStr = tmpStr & "\"
' ------------------
ShellExecute Me.hWnd, "open", "net", "stop Themes", vbNullString, SW_HIDE
Sleep 2000
' ------------------
If OSVERSIONINFOX.dwMajorVersion = 5 And OSVERSIONINFOX.dwMinorVersion < 2 Then
    '如果是Windows XP
    retV = RegisterUserApiHookXP(DLLhwnd, DLLFunDre)    '注册回调函数
Else
    ' 如果是Windows 2003
    HookAPIRegInfoX2003.m_size = Len(HookAPIRegInfoX2003)
    HookAPIRegInfoX2003.m_dllname1 = StrPtr("hxwdllwx.dll")
    HookAPIRegInfoX2003.m_dllname2 = StrPtr("hxwdllwx.dll")
    HookAPIRegInfoX2003.m_funname1 = StrPtr("DLLstopHOOK")
    HookAPIRegInfoX2003.m_funname2 = StrPtr("DLLstopHOOK")
    retV = RegisterUserApiHook2003(HookAPIRegInfoX2003)
    If retV = 0 Then
        HookAPIRegInfoX2003.m_size = Len(HookAPIRegInfoX2003)
        HookAPIRegInfoX2003.m_dllname1 = StrPtr(tmpStr & "hxwdllwx.dll")
        HookAPIRegInfoX2003.m_dllname2 = StrPtr(tmpStr & "hxwdllwx.dll")
        HookAPIRegInfoX2003.m_funname1 = StrPtr("DLLstopHOOK")
        HookAPIRegInfoX2003.m_funname2 = StrPtr("DLLstopHOOK")
        retV = RegisterUserApiHook2003(HookAPIRegInfoX2003)
    End If
End If
If retV = 0 Then MsgBox "程序初始化失败！错误消息为：" & vbCrLf & GetRetMessage(Err.LastDllError, FORMAT_MESSAGE_FROM_SYSTEM), vbCritical, "注入失败！"
DLLstartHOOK Me.hWnd '初始化DLL
End Sub

Private Sub Form_Unload(Cancel As Integer)
DLLsetHOOKState False
UnregisterUserApiHook  '退出时卸载HOOK
Call SetWindowLong(Me.hWnd, GWL_WNDPROC, PrevWndProc)  '还原子类化窗口
FreeLibrary DLLhwnd
ShellExecute Me.hWnd, "open", "net", "start Themes", vbNullString, SW_HIDE
End Sub

Private Sub Text1_Change()
Text1.SelStart = Len(Text1.Text)
End Sub
