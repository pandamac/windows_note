VERSION 5.00
Begin VB.Form Form4 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "帮助与关于"
   ClientHeight    =   4275
   ClientLeft      =   45
   ClientTop       =   435
   ClientWidth     =   4560
   LinkTopic       =   "Form4"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4275
   ScaleWidth      =   4560
   StartUpPosition =   3  '窗口缺省
   Begin VB.TextBox Text1 
      Appearance      =   0  'Flat
      BackColor       =   &H8000000F&
      BorderStyle     =   0  'None
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   9
         Charset         =   134
         Weight          =   400
         Underline       =   -1  'True
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H000000FF&
      Height          =   270
      Left            =   1080
      Locked          =   -1  'True
      MouseIcon       =   "Form4.frx":0000
      MousePointer    =   99  'Custom
      TabIndex        =   3
      Text            =   "http://www.huoxingwen.com"
      Top             =   1440
      Width           =   2415
   End
   Begin VB.CommandButton Command1 
      Caption         =   "确定"
      Height          =   375
      Left            =   1440
      TabIndex        =   0
      Top             =   3360
      Width           =   1335
   End
   Begin VB.Label Label4 
      Caption         =   "查看使用说明"
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   9
         Charset         =   134
         Weight          =   400
         Underline       =   -1  'True
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FF0000&
      Height          =   255
      Left            =   1560
      MouseIcon       =   "Form4.frx":030A
      MousePointer    =   99  'Custom
      TabIndex        =   5
      Top             =   2880
      Width           =   1095
   End
   Begin VB.Label Label3 
      Caption         =   "火星文转换器V2.0版集成由火星文官网提供的最新数据库，最后更新于2008年3月7日。看懂火星文，听懂火星语，欢迎访问火星文官方网站！"
      Height          =   615
      Left            =   360
      TabIndex        =   4
      Top             =   1800
      Width           =   4095
   End
   Begin VB.Label Label2 
      Caption         =   "火星文官方网站出品"
      Height          =   255
      Left            =   1320
      TabIndex        =   2
      Top             =   960
      Width           =   1815
   End
   Begin VB.Label Label1 
      Caption         =   "火星文转换器 V2.0版"
      BeginProperty Font 
         Name            =   "宋体"
         Size            =   12
         Charset         =   134
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FF0000&
      Height          =   255
      Left            =   960
      TabIndex        =   1
      Top             =   480
      Width           =   2655
   End
End
Attribute VB_Name = "Form4"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Command1_Click()
Unload Me
End Sub


Private Sub Label4_Click()
ShellExecute Me.hwnd, "open", "使用说明.txt", "", "", 5
End Sub

Private Sub Text1_Click()
ShellExecute Me.hwnd, "open", "http://www.huoxingwen.com", "", "", 5
End Sub
