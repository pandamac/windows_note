VERSION 5.00
Begin VB.Form Form3 
   Caption         =   "自动聊天工具参数设置"
   ClientHeight    =   3525
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   5775
   LinkTopic       =   "Form3"
   MaxButton       =   0   'False
   ScaleHeight     =   3525
   ScaleWidth      =   5775
   StartUpPosition =   3  '窗口缺省
   Begin VB.CommandButton Command2 
      Caption         =   "取消"
      Height          =   375
      Left            =   3000
      TabIndex        =   12
      Top             =   3000
      Width           =   855
   End
   Begin VB.CommandButton Command1 
      Caption         =   "确定"
      Height          =   375
      Left            =   1560
      TabIndex        =   11
      Top             =   3000
      Width           =   855
   End
   Begin VB.Frame Frame2 
      Caption         =   "其它参数"
      Height          =   975
      Left            =   240
      TabIndex        =   6
      Top             =   1800
      Width           =   5175
      Begin VB.TextBox Text4 
         Appearance      =   0  'Flat
         Height          =   270
         Left            =   1920
         TabIndex        =   10
         Top             =   600
         Width           =   735
      End
      Begin VB.TextBox Text3 
         Appearance      =   0  'Flat
         Height          =   270
         Left            =   1920
         TabIndex        =   8
         Top             =   240
         Width           =   735
      End
      Begin VB.Label Label5 
         Caption         =   "退出提问状态概率："
         Height          =   255
         Left            =   240
         TabIndex        =   9
         Top             =   600
         Width           =   1695
      End
      Begin VB.Label Label4 
         Caption         =   "进入提问状态概率："
         Height          =   255
         Left            =   240
         TabIndex        =   7
         Top             =   240
         Width           =   1695
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "QQ自动聊天设置"
      Height          =   1335
      Left            =   240
      TabIndex        =   0
      Top             =   240
      Width           =   5175
      Begin VB.TextBox Text2 
         Height          =   270
         Left            =   1680
         TabIndex        =   5
         Top             =   840
         Width           =   2415
      End
      Begin VB.TextBox Text1 
         Appearance      =   0  'Flat
         Height          =   270
         Left            =   1680
         TabIndex        =   2
         Top             =   360
         Width           =   975
      End
      Begin VB.Label Label3 
         Caption         =   "呼出QQ消息热键："
         Height          =   255
         Left            =   240
         TabIndex        =   4
         Top             =   840
         Width           =   1455
      End
      Begin VB.Label Label2 
         Caption         =   "毫秒 (1000毫秒=1秒)"
         Height          =   255
         Left            =   2760
         TabIndex        =   3
         Top             =   360
         Width           =   1935
      End
      Begin VB.Label Label1 
         Caption         =   "QQ消息轮询间隔："
         Height          =   255
         Left            =   240
         TabIndex        =   1
         Top             =   360
         Width           =   1455
      End
   End
End
Attribute VB_Name = "Form3"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private temp_hotkey As String

Private Sub Command1_Click()
On Error Resume Next
Form1.Timer1.Interval = CLng(Text1.Text)
qqchat_hotkey = temp_hotkey
rnd_instudy = CInt(Text3.Text)
rnd_outstudy = CInt(Text4.Text)
Unload Me
End Sub

Private Sub Command2_Click()
Unload Me
End Sub

Private Sub Form_Load()
Text1.Text = CStr(Form1.Timer1.Interval)
Text3.Text = CStr(rnd_instudy)
Text4.Text = CStr(rnd_outstudy)
temp_hotkey = qqchat_hotkey
Text2.Text = key2show(temp_hotkey)
End Sub

Private Function key2show(tmp_str) As String
Dim out_str As String
tmp_array = Split(tmp_str, "|")
out_str = ""
For i = 0 To UBound(tmp_array)
    If out_str = "" Then
       out_str = keyCoadToChar(CLng(tmp_array(i)))
    Else
       out_str = out_str & "+" & keyCoadToChar(CLng(tmp_array(i)))
    End If
Next
key2show = out_str
End Function

Private Function keyCoadToChar(mycoad) As String
Dim str_out As String
str_out = Chr(mycoad)
If mycoad = VK_Ctrl Then str_out = "Ctrl"
If mycoad = VK_ALT Then str_out = "Alt"
If mycoad = VK_SHIFT Then str_out = "Shift"
If mycoad = VK_F1 Then str_out = "F1"
If mycoad = VK_F2 Then str_out = "F2"
If mycoad = VK_F3 Then str_out = "F3"
If mycoad = VK_F4 Then str_out = "F4"
If mycoad = VK_F5 Then str_out = "F5"
If mycoad = VK_F6 Then str_out = "F6"
If mycoad = VK_F7 Then str_out = "F7"
If mycoad = VK_F8 Then str_out = "F8"
If mycoad = VK_F9 Then str_out = "F9"
If mycoad = VK_F10 Then str_out = "F10"
If mycoad = VK_F11 Then str_out = "F11"
If mycoad = VK_F12 Then str_out = "F12"
If mycoad = VK_ESC Then str_out = "Esc"
keyCoadToChar = str_out
End Function

Private Sub Text2_KeyDown(KeyCode As Integer, Shift As Integer)
Dim strKey As String
If Shift = 1 Then strKey = CStr(VK_SHIFT) & " "
If Shift = 2 Then strKey = CStr(VK_Ctrl) & " "
If Shift = 4 Then strKey = CStr(VK_ALT) & " "
If Shift = 3 Then strKey = CStr(VK_Ctrl) & " " & CStr(VK_SHIFT) & " "
If Shift = 5 Then strKey = CStr(VK_SHIFT) & " " & CStr(VK_ALT) & " "
If Shift = 6 Then strKey = CStr(VK_Ctrl) & " " & CStr(VK_ALT) & " "
If Shift = 7 Then strKey = CStr(VK_Ctrl) & " " & CStr(VK_SHIFT) & " " & CStr(VK_ALT) & " "
If KeyCode <> VK_SHIFT And KeyCode <> VK_Ctrl And KeyCode <> VK_ALT Then strKey = strKey & CStr(KeyCode)
strKey = Trim(strKey)
strKey = Replace(strKey, " ", "|")
Text2.Text = key2show(strKey)
Text2.SelStart = Len(Text2.Text)
If KeyCode >= 112 And KeyCode <= 123 Then temp_hotkey = strKey
If Shift > 0 And (KeyCode <> VK_SHIFT And KeyCode <> VK_Ctrl And KeyCode <> VK_ALT) Then temp_hotkey = strKey
End Sub

Private Sub Text2_KeyUp(KeyCode As Integer, Shift As Integer)
Text2.Text = key2show(temp_hotkey)
End Sub
