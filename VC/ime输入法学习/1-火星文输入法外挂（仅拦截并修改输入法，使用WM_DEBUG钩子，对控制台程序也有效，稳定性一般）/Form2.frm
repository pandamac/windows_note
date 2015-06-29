VERSION 5.00
Begin VB.Form Form2 
   Caption         =   "我来当老师！"
   ClientHeight    =   4395
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   9870
   Icon            =   "Form2.frx":0000
   LinkTopic       =   "Form2"
   MaxButton       =   0   'False
   ScaleHeight     =   4395
   ScaleWidth      =   9870
   StartUpPosition =   3  '窗口缺省
   Begin VB.CommandButton Command6 
      Caption         =   "关闭本窗口"
      Height          =   375
      Left            =   7320
      TabIndex        =   20
      Top             =   3720
      Width           =   2175
   End
   Begin VB.CommandButton Command5 
      Caption         =   "保存"
      Enabled         =   0   'False
      Height          =   375
      Left            =   2400
      TabIndex        =   19
      Top             =   3720
      Width           =   1815
   End
   Begin VB.CommandButton Command4 
      Caption         =   "添加"
      Height          =   375
      Left            =   240
      TabIndex        =   18
      Top             =   3720
      Width           =   1815
   End
   Begin VB.CommandButton Command3 
      Caption         =   "删除此条"
      Height          =   375
      Left            =   8520
      TabIndex        =   17
      Top             =   3120
      Width           =   975
   End
   Begin VB.CommandButton Command2 
      Caption         =   "提交修改"
      Height          =   375
      Left            =   7200
      TabIndex        =   16
      Top             =   3120
      Width           =   1095
   End
   Begin VB.TextBox Text4 
      Appearance      =   0  'Flat
      Height          =   270
      Left            =   5520
      TabIndex        =   14
      Text            =   "1"
      Top             =   3240
      Width           =   615
   End
   Begin VB.CommandButton Command1 
      Caption         =   "转到"
      Height          =   375
      Index           =   4
      Left            =   4560
      TabIndex        =   12
      Top             =   3120
      Width           =   615
   End
   Begin VB.CommandButton Command1 
      Caption         =   "最后一条"
      Height          =   375
      Index           =   3
      Left            =   3240
      TabIndex        =   11
      Top             =   3120
      Width           =   975
   End
   Begin VB.CommandButton Command1 
      Caption         =   "下一条"
      Height          =   375
      Index           =   2
      Left            =   2280
      TabIndex        =   10
      Top             =   3120
      Width           =   855
   End
   Begin VB.CommandButton Command1 
      Caption         =   "上一条"
      Height          =   375
      Index           =   1
      Left            =   1320
      TabIndex        =   9
      Top             =   3120
      Width           =   855
   End
   Begin VB.CommandButton Command1 
      Caption         =   "第一条"
      Height          =   375
      Index           =   0
      Left            =   240
      TabIndex        =   8
      Top             =   3120
      Width           =   975
   End
   Begin VB.TextBox Text3 
      Height          =   375
      Left            =   1560
      TabIndex        =   7
      Top             =   2520
      Width           =   7935
   End
   Begin VB.TextBox Text2 
      Height          =   375
      Left            =   2400
      TabIndex        =   5
      Top             =   1800
      Width           =   7095
   End
   Begin VB.TextBox Text1 
      Height          =   375
      Left            =   2040
      TabIndex        =   3
      Top             =   1200
      Width           =   7455
   End
   Begin VB.Label Label8 
      Caption         =   "0/0"
      Height          =   255
      Left            =   4560
      TabIndex        =   21
      Top             =   3840
      Width           =   2415
   End
   Begin VB.Label Label7 
      Caption         =   "条"
      Height          =   255
      Left            =   6240
      TabIndex        =   15
      Top             =   3240
      Width           =   255
   End
   Begin VB.Label Label6 
      Caption         =   "第"
      Height          =   255
      Left            =   5280
      TabIndex        =   13
      Top             =   3240
      Width           =   255
   End
   Begin VB.Label Label5 
      Caption         =   "答(answer)："
      Height          =   255
      Left            =   240
      TabIndex        =   6
      Top             =   2640
      Width           =   1215
   End
   Begin VB.Label Label4 
      Caption         =   "且不包含如下关键词:"
      Height          =   375
      Left            =   480
      TabIndex        =   4
      Top             =   1920
      Width           =   1815
   End
   Begin VB.Label Label3 
      Caption         =   "包含如下关键词:"
      Height          =   255
      Left            =   480
      TabIndex        =   2
      Top             =   1320
      Width           =   1455
   End
   Begin VB.Label Label2 
      Caption         =   "问(ask)："
      Height          =   255
      Left            =   240
      TabIndex        =   1
      Top             =   960
      Width           =   855
   End
   Begin VB.Label Label1 
      Caption         =   $"Form2.frx":628A
      Height          =   735
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   9615
   End
End
Attribute VB_Name = "Form2"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private faq_id As Long, faq_const As Long

Private Sub Command1_Click(Index As Integer)
sql = "select * from qqchat_faq "
rs_pub.open sql, conn, 1, 1
On Error Resume Next
If Index = 0 Then
   Command1(1).Enabled = False
   If rs_pub.RecordCount = 0 Then faq_const = 0 Else faq_const = 1
   If faq_const >= rs_pub.RecordCount Then Command1(2).Enabled = False Else Command1(2).Enabled = True
End If

If Index = 1 Then
   faq_const = faq_const - 1
   If faq_const <= 1 Then Command1(1).Enabled = False
   If faq_const < rs_pub.RecordCount Then Command1(2).Enabled = True
   rs_pub.Move faq_const - 1
End If

If Index = 2 Then
   faq_const = faq_const + 1
   If faq_const >= rs_pub.RecordCount Then Command1(2).Enabled = False
   If faq_const > 1 Then Command1(1).Enabled = True
   rs_pub.Move faq_const - 1
End If

If Index = 3 Then
   rs_pub.movelast
   Command1(2).Enabled = False
   faq_const = rs_pub.RecordCount
   If faq_const > 1 Then Command1(1).Enabled = True Else Command1(1).Enabled = False
End If

If Index = 4 Then
   If Not IsNumeric(Text4.Text) Then Text4.Text = "1"
   faq_const = CLng(Text4.Text)
   If faq_const > rs_pub.RecordCount Then faq_const = rs_pub.RecordCount
   If faq_const < 1 Then faq_const = 1
   If faq_const >= rs_pub.RecordCount Then Command1(2).Enabled = False Else Command1(2).Enabled = True
   If faq_const > 1 Then Command1(1).Enabled = True Else Command1(1).Enabled = False
   rs_pub.Move faq_const - 1
End If

If rs_pub("ask") <> "" Then Text1.Text = rs_pub("ask") Else Text1.Text = ""
If rs_pub("notask") <> "" Then Text2.Text = rs_pub("notask") Else Text2.Text = ""
If rs_pub("answer") <> "" Then Text3.Text = rs_pub("answer") Else Text3.Text = ""
faq_id = rs_pub("id")
Label8.Caption = CStr(faq_const) & "/" & CStr(rs_pub.RecordCount) & "条"
rs_pub.Close
End Sub

Private Sub Command2_Click()
Text1.Text = Trim(Text1.Text)
Text2.Text = Trim(Text2.Text)
Text3.Text = Trim(Text3.Text)
If Text1.Text = "" Or Text3.Text = "" Then Exit Sub
sql = "select * from qqchat_faq where [id]=" & CStr(faq_id)
rs_pub.open sql, conn, 1, 3
rs_pub("ask") = Text1.Text
rs_pub("notask") = Text2.Text
rs_pub("answer") = Text3.Text
rs_pub.Update
rs_pub.Close
MsgBox "数据修改成功！"
End Sub

Private Sub Command3_Click()
If MsgBox("真的要删除此条记录吗？", vbYesNo, "确认删除") = vbYes Then
   conn.execute "delete from qqchat_faq where [id]=" & CStr(faq_id)
   Command1_Click 0
   MsgBox "删除成功！"
End If
End Sub

Private Sub Command4_Click()
Command4.Enabled = False
Command5.Enabled = True
Text1.Text = ""
Text2.Text = ""
Text3.Text = ""
End Sub

Private Sub Command5_Click()
Text1.Text = Trim(Text1.Text)
Text2.Text = Trim(Text2.Text)
Text3.Text = Trim(Text3.Text)
If Text1.Text = "" Or Text3.Text = "" Then Exit Sub
sql = "select * from qqchat_faq "
rs_pub.open sql, conn, 1, 3
rs_pub.addnew
rs_pub("ask") = Text1.Text
rs_pub("notask") = Text2.Text
rs_pub("answer") = Text3.Text
rs_pub.Update
rs_pub.Close
Command4.Enabled = True
Command5.Enabled = False
MsgBox "数据保存成功！"
Command1_Click 3
End Sub

Private Sub Command6_Click()
Unload Me
End Sub

Private Sub Form_Load()
Command1_Click 0
End Sub
