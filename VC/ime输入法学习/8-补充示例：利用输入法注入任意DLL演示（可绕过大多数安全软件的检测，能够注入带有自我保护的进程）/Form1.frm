VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "利用输入法向进程注入DLL"
   ClientHeight    =   5220
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   6840
   LinkTopic       =   "Form1"
   ScaleHeight     =   5220
   ScaleWidth      =   6840
   StartUpPosition =   3  '窗口缺省
   Begin VB.TextBox Text6 
      Height          =   270
      Left            =   5520
      TabIndex        =   17
      Text            =   "0"
      Top             =   2680
      Width           =   375
   End
   Begin VB.CommandButton Command4 
      Caption         =   "卸载已安装的服务输入法"
      Height          =   375
      Left            =   2880
      TabIndex        =   15
      Top             =   4680
      Width           =   2295
   End
   Begin VB.TextBox Text5 
      Height          =   270
      Left            =   4680
      TabIndex        =   13
      Text            =   "0"
      Top             =   2400
      Width           =   375
   End
   Begin VB.CommandButton Command3 
      Caption         =   "停止继续注入"
      Height          =   375
      Left            =   240
      TabIndex        =   11
      Top             =   4680
      Width           =   1935
   End
   Begin VB.TextBox Text4 
      Height          =   270
      Left            =   6000
      TabIndex        =   10
      Text            =   "0"
      Top             =   2160
      Width           =   495
   End
   Begin VB.TextBox Text3 
      Height          =   270
      Left            =   6000
      TabIndex        =   8
      Text            =   "0"
      Top             =   1680
      Width           =   495
   End
   Begin VB.TextBox Text2 
      Height          =   270
      Left            =   6000
      TabIndex        =   6
      Text            =   "0"
      Top             =   1275
      Width           =   495
   End
   Begin VB.TextBox Text1 
      Height          =   270
      Left            =   1800
      TabIndex        =   3
      Text            =   "hxwdllwx.dll"
      Top             =   840
      Width           =   4695
   End
   Begin VB.CommandButton Command1 
      Caption         =   "第一步：指定要注入的DLL及其回调函数参数"
      Height          =   495
      Left            =   240
      TabIndex        =   1
      Top             =   240
      Width           =   3975
   End
   Begin VB.CommandButton Command2 
      Caption         =   "第二步：安装服务输入法并注入指定DLL"
      Height          =   495
      Left            =   240
      TabIndex        =   0
      Top             =   3480
      Width           =   4095
   End
   Begin VB.Label Label8 
      Caption         =   "当切换到目标输入法时自动切换到下一个输入法（0-否，1-是）："
      Height          =   255
      Left            =   240
      TabIndex        =   16
      Top             =   2760
      Width           =   5295
   End
   Begin VB.Label Label7 
      Caption         =   "后续操作（可选）："
      Height          =   255
      Left            =   240
      TabIndex        =   14
      Top             =   4320
      Width           =   3495
   End
   Begin VB.Label Label6 
      Caption         =   "输入法退出时是否同时卸载注入的DLL（0-是，1-否）："
      Height          =   255
      Left            =   240
      TabIndex        =   12
      Top             =   2450
      Width           =   4455
   End
   Begin VB.Label Label5 
      Caption         =   "参数3："
      ForeColor       =   &H00FF0000&
      Height          =   255
      Left            =   5280
      TabIndex        =   9
      Top             =   2205
      Width           =   735
   End
   Begin VB.Label Label4 
      Caption         =   "参数2："
      ForeColor       =   &H00FF0000&
      Height          =   255
      Left            =   5280
      TabIndex        =   7
      Top             =   1725
      Width           =   735
   End
   Begin VB.Label Label3 
      Caption         =   "参数1："
      ForeColor       =   &H00FF0000&
      Height          =   255
      Left            =   5280
      TabIndex        =   5
      Top             =   1320
      Width           =   735
   End
   Begin VB.Label Label2 
      Caption         =   $"Form1.frx":0000
      Height          =   975
      Left            =   240
      TabIndex        =   4
      Top             =   1200
      Width           =   4935
   End
   Begin VB.Label Label1 
      Caption         =   "DLL名称及路径："
      Height          =   255
      Left            =   240
      TabIndex        =   2
      Top             =   840
      Width           =   1455
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Private Sub Command1_Click()
If Text1.Text = "" Then
    MsgBox "请输入DLL文件名！"
    Text1.SetFocus
    Exit Sub
End If
MsgBox "配置完毕！"
End Sub

Private Sub Command2_Click()
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
    If ImeHKLString <> "" Then MSetIMEIntoFirst ImeHKLString    '如果安装成功，则将指定输入法移动到输入法列表的第一项
    MSetIMEIsDefInput retV   '将指定输入法设置为系统默认输入法，应用程序一打开就加载
End If

If Dir(GSystemPath & GImeFileName & ".ime") <> "" Then
' 如果输入法被安装到系统目录，则尝试将其初始化
    IMESetPubString Text1.Text, Val(Text5.Text), Val(Text6.Text), Val(Text2.Text), Val(Text3.Text), Val(Text4.Text)
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
    MsgBox "抱歉，输入法安装失败。您可能没有管理员权限，或者该输入法服务已经安装。", vbCritical
Else
    MsgBox "输入法宿主已成功安装，指定DLL已开始尝试注入所有窗口程序。" & vbCrLf & "输入法句柄是：" & retV, vbExclamation
End If
End Sub

Private Sub Command3_Click()
On Error Resume Next
If Dir(GSystemPath & "imedllhost09.ime") <> "" Then
' 清空输入法配置，停止继续注入
    IMEClearPubString
End If
End Sub

Private Sub Command4_Click()
Dim retV As Long
MQuitIMEIsDefInput GSysDefIme    '首先还原系统默认的键盘布局
MActiveIMEForWindows GSysDefIme, 0  '将所有顶级窗口重设为默认输入法
If ImeHKL <> 0 Then
    retV = MUnLoadIMEByHKL(ImeHKL)
    If retV = 0 Then
        MsgBox "服务输入法卸载失败。可能该输入法正在使用且被锁定。", vbCritical
    Else
        MsgBox "服务输入法已成功卸载。", vbExclamation
    End If
Else
    retV = MsgBox("服务输入法可能不是本次运行时安装的。是否要遍历系统所有的输入法并尝试卸载名称为“Windows标准输入法扩展服务”的输入法？", vbExclamation Or vbYesNo Or vbDefaultButton1)
    If retV = vbYes Then
        retV = MUnLoadIMEByName(GImeFileName)   '尝试通过文件名卸载输入法
        If retV = 0 Then
            MsgBox "服务输入法卸载失败。可能该输入法正在使用且被锁定，或该输入法不存在", vbCritical
        Else
            MsgBox "服务输入法已成功卸载。", vbExclamation
        End If
    End If
End If
If ImeHKLString <> "" Then
    MDeleteRegIme ImeHKLString
End If
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
Text1.Text = GPathStr & "hxwdllwx.dll"
End Sub
