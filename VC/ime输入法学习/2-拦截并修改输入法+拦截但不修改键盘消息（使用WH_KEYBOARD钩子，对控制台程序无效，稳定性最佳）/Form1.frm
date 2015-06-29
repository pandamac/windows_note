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
WM_HXWDLLWX_QQBTX = RegisterWindowMessage("WM_HXWDLLWX_QQBTX")  '注册自定义消息
WM_HXWDLLWX_HOOKKEY = RegisterWindowMessage("WM_HXWDLLWX_HOOKKEY")

PrevWndProc = SetWindowLong(Me.hwnd, GWL_WNDPROC, AddressOf SubWndProc) '子类化窗口，以便能处理DLL发出的自定义消息
DLLstartHOOK Me.hwnd '初始化DLL
End Sub

Private Sub Form_Unload(Cancel As Integer)
DLLstopHOOK  '退出时卸载HOOK
Call SetWindowLong(Me.hwnd, GWL_WNDPROC, PrevWndProc)  '还原子类化窗口
End Sub
