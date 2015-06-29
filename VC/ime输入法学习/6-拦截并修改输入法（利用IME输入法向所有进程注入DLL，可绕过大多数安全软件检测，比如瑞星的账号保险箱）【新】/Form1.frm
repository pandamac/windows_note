VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "输入法HOOK - 利用输入法向进程注入DLL"
   ClientHeight    =   4155
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   6840
   LinkTopic       =   "Form1"
   ScaleHeight     =   4155
   ScaleWidth      =   6840
   StartUpPosition =   3  '窗口缺省
   Begin VB.CommandButton Command1 
      Caption         =   "打开拦截"
      Height          =   375
      Left            =   960
      TabIndex        =   2
      Top             =   3600
      Width           =   1455
   End
   Begin VB.CommandButton Command2 
      Caption         =   "关闭拦截"
      Height          =   375
      Left            =   3960
      TabIndex        =   1
      Top             =   3600
      Width           =   1455
   End
   Begin VB.TextBox Text1 
      Height          =   2895
      Left            =   240
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   0
      Top             =   480
      Width           =   6255
   End
   Begin VB.Label Label1 
      Caption         =   "先点击“打开拦截”按钮，然后在任何其它程序用输入法输入汉字试试看。"
      Height          =   255
      Left            =   240
      TabIndex        =   3
      Top             =   120
      Width           =   6135
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
' ---------获得系统路径和当前路径------
GSystemPath = Environ("windir")
If Right(GSystemPath, 1) <> "\" Then GSystemPath = GSystemPath & "\"
GSystemPath = GSystemPath & "system32\"
If Dir(GSystemPath) = "" Then GSystemPath = Environ("windir") & "\system\"
GPathStr = App.Path
If Right(GPathStr, 1) <> "\" Then GPathStr = GPathStr & "\"
' -----------------------------------
SystemParametersInfo SPI_GETDEFAULTINPUTLANG, 0, GSysDefIme, 0 '得到系统默认的输入法的句柄，用于退出时还原
' -----------------------------------
GImeFileName = "imedllhost09"    '设置输入法文件名，不含扩展名
CilentDllPath = GPathStr & "hxwdllwx.dll"    '欲注入的目标DLL完整路径
'-----------------------------------
WM_HXWDLLWX_QQBTX = RegisterWindowMessage("WM_HXWDLLWX_QQBTX")  '注册自定义消息
DLLstartHOOK Me.hWnd '初始化目标DLL
' ---------------------------------
On Error Resume Next
Dim retV As Long
retV = MSetUpIME(GImeFileName)   '安装指定的输入法
If retV = 0 Then
    retV = MImeFindByName(GImeFileName)   '如果安装失败，查询该输入法是否已经安装
End If
If retV <> 0 Then ImeHKL = retV   '保存输入法句柄到全局变量

If retV <> 0 Then
    '如果安装成功，则将此输入法设置为默认输入法，让程序一打开就加载
    ImeHKLString = MGetIMEHwndString(retV)   '取得输入法的句柄字符串
    'If ImeHKLString <> "" Then MSetIMEIntoFirst ImeHKLString    '如果安装成功，则将指定输入法移动到输入法列表的第一项（这一步操作可以去掉，免得影响用户的输入法顺序）
    MSetIMEIsDefInput retV   '将指定输入法设置为系统默认输入法，应用程序一打开就加载
End If

If Dir(GSystemPath & GImeFileName & ".ime") <> "" Then
' 如果输入法被安装到系统目录，则尝试将其初始化，向其传递欲注入的目标DLL名称
    IMESetPubString CilentDllPath, 0, 0, 0, 0, 0
    '该函数的参数含义从左到右依次是:
    '欲注入的目标DLL完整路径。
    '当输入法退出时，是否同时卸载目标DLL 0-是，1-否
    '当切换至该输入法时，是否直接切换到下一个输入法（这样该输入法就好像被跳过了，可最小限度影响用户的输入法顺序） 0-否，1-是
    '最后3个参数是传递给目标DLL的回调函数（函数名称必须为RunDllHostCallBack）的参数
End If

Dim hIme As Long
If retV <> 0 Then
    '如果安装成功，则在所有已经运行的程序中激活此输入法。这样可实现向所有窗口同时注入目标DLL
    MActiveIMEForWindows retV, 1   '第一个参数为输入法句柄。第二个参数为激活强度，0-只尝试在顶级窗口中激活，1-在所有窗口中激活
Else
    hIme = MImeFindByName(GImeFileName)
    If hIme <> 0 Then MActiveIMEForWindows hIme, 1
End If

If retV = 0 Then
    'MsgBox "抱歉，输入法安装失败。您可能没有管理员权限，或者该输入法服务已经安装。", vbCritical
End If
' ---------------------------------
PrevWndProc = SetWindowLong(Me.hWnd, GWL_WNDPROC, AddressOf SubWndProc) '子类化窗口，以便能处理DLL发出的自定义消息
End Sub

Private Sub Form_Unload(Cancel As Integer)
DLLsetHOOKState False    '退出时一定要正确关闭目标DLL的HOOK，不然所有程序都会出问题
Call SetWindowLong(Me.hWnd, GWL_WNDPROC, PrevWndProc)  '还原子类化窗口
' --------------------------
On Error Resume Next
If Dir(GSystemPath & GImeFileName & ".ime") <> "" Then
    ' 清空输入法配置，停止继续注入
    IMEClearPubString
End If
' --------------------------
Dim retV As Long
MQuitIMEIsDefInput GSysDefIme    '首先还原系统默认的键盘布局
MActiveIMEForWindows GSysDefIme, 0  '将所有顶级窗口重设为默认输入法
If ImeHKL <> 0 Then
    retV = MUnLoadIMEByHKL(ImeHKL)
    If retV = 0 Then
        'MsgBox "服务输入法卸载失败。可能该输入法正在使用且被锁定。", vbCritical
    Else
        'MsgBox "服务输入法已成功卸载。", vbExclamation
    End If
Else
    'retV = MsgBox("服务输入法可能不是本次运行时安装的。是否要遍历系统所有的输入法并尝试卸载名称为“Windows标准输入法扩展服务”的输入法？", vbExclamation Or vbYesNo Or vbDefaultButton1)
    retV = vbYes
    If retV = vbYes Then
        retV = MUnLoadIMEByName(GImeFileName)   '尝试通过文件名卸载输入法
        If retV = 0 Then
            'MsgBox "服务输入法卸载失败。可能该输入法正在使用且被锁定，或该输入法不存在", vbCritical
        Else
            'MsgBox "服务输入法已成功卸载。", vbExclamation
        End If
    End If
End If
If ImeHKLString <> "" Then
    MDeleteRegIme ImeHKLString   '从注册表中删除输入法残留信息
End If
End Sub

Private Sub Text1_Change()
Text1.SelStart = Len(Text1.Text)
End Sub
