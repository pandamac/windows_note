Attribute VB_Name = "Module2"
Public rnd_instudy As Integer, rnd_outstudy As Integer, qqchat_hotkey As String
Private qqchat_mode As Integer   ' 0-聊天模式，1-学习模式
Private qqchat_firstin As Integer, lastAskStr As String
Private qqchat_table_instudy As String, qqchat_table_noanswer As String, qqchat_table_outstudy As String

Sub qqchat_init()
sql = "select * from mysetting where [id]>=2 order by [id] "
rs_pub.open sql, conn, 1, 1
temp_array = Split(rs_pub("myset1"), "|")
rs_pub.movenext
qqchat_hotkey = rs_pub("myset1")
rs_pub.Close
Form1.Timer1.Interval = CLng(temp_array(0))
rnd_instudy = CInt(temp_array(1))
rnd_outstudy = CInt(temp_array(2))
Form1.Check4.Value = CInt(temp_array(3))
Form1.Check5.Value = CInt(temp_array(4))
qqchat_mode = 0
qqchat_firstin = 0
init_qqchat_table
qqchat_fun1
End Sub

Sub init_qqchat_table()
sql = "select * from qqchat_instudy"
rs_pub.open sql, conn, 1, 1
Do While Not rs_pub.EOF
qqchat_table_instudy = qqchat_table_instudy & rs_pub("instudy") & vbCrLf
rs_pub.movenext
Loop
rs_pub.Close

sql = "select * from qqchat_noanswer where id=1"
rs_pub.open sql, conn, 1, 1
qqchat_table_noanswer = rs_pub("noanswer")
rs_pub.Close

sql = "select * from qqchat_outstudy"
rs_pub.open sql, conn, 1, 1
Do While Not rs_pub.EOF
qqchat_table_outstudy = qqchat_table_outstudy & rs_pub("outstudy") & vbCrLf
rs_pub.movenext
Loop
rs_pub.Close
End Sub

Sub qqchat_fun1()
Dim tmp_str As String
sql = "select * from qqchat_firstask"
rs_pub.open sql, conn, 1, 1
Do While Not rs_pub.EOF
tmp_str = tmp_str & rs_pub("firstask") & vbCrLf
rs_pub.movenext
Loop
rs_pub.Close
str_array = Split(tmp_str, vbCrLf)
tmp_id = Int(UBound(str_array) * Rnd)
Form1.Text10.Text = str_array(tmp_id)
End Sub

Sub qqchat_kernel()
On Error Resume Next
If qqchat_mode = 0 Then
qqchat_domode0
Else
qqchat_domode1
End If
If Form1.Check5.Value = 1 Then Form1.Text10.Text = pub_fun_jt2bt(Form1.Text10.Text)
qqchat_jilu
Form1.Text11.Text = ""
End Sub

Sub qqchat_jilu()
you$ = Form1.Text11.Text
chat$ = Form1.Text10.Text
you$ = "你说：" + you$
chat$ = "计算机说：" + chat$
Open "聊天记录.txt" For Append As #1
mychat$ = you$ + vbCrLf + chat$ + vbCrLf + Date$ + " " + Time$ + vbCrLf + vbCrLf
Print #1, Date$ + " " + Time$: Print #1, you$: Print #1, chat$
Close #1
Form1.Text9.Text = Form1.Text9.Text + mychat$
Form1.Text9.SelStart = Len(Form1.Text9.Text)
End Sub

Sub qqchat_domode0()
word$ = Trim(Form1.Text11.Text)
If word$ = "" Then Form1.Text10.Text = "": Exit Sub
tmp_array = Split(qqchat_table_instudy, vbCrLf)
For i = 0 To UBound(tmp_array) - 1
    If InStr(1, word$, tmp_array(i)) > 0 And (Len(word$) - Len(tmp_array(i)) <= 4) Then qqchat_mode = 1
Next
If Int(Rnd * rnd_instudy) = 1 Then qqchat_mode = 1  '一定概率自动转换为学习状态

If qqchat_mode = 1 Then
qqchat_firstin = 1
qqchat_domode1
Else
Form1.Text10.Text = qqchat_talk$(word$)
End If
End Sub

Function qqchat_talk$(word$)
Dim maxFixNum As Long, bestAnswer As String
Dim str1 As String, str2 As String, tmp_flag As Boolean, i As Long, tmp_long As Long, flag_fix As Boolean
maxFixNum = 0
bestAnswer = ""
sql = "select * from qqchat_faq "
rs_pub.open sql, conn, 1, 1
Do While Not rs_pub.EOF
str1 = Trim(rs_pub("ask"))
str2 = Trim(rs_pub("answer"))
If str1 <> "" And str2 <> "" Then
   tmp_flag = True
   If rs_pub("notask") <> "" Then
      notask_array = Split(rs_pub("notask"), " ")
      For i = 0 To UBound(notask_array)
          If notask_array(i) <> "" Then
             If InStr(1, word$, notask_array(i)) > 0 Then tmp_flag = False
          End If
      Next
   End If
   
   If tmp_flag Then
      tmp_array = Split(str1, " ")
      tmp_long = UBound(tmp_array)
      If tmp_long + 1 >= maxFixNum Then
         flag_fix = True
         For i = 0 To tmp_long
             If Not InStr(1, word$, tmp_array(i)) > 0 Then
                flag_fix = False
                Exit For
             End If
         Next
         If flag_fix Then
            If tmp_long + 1 > maxFixNum Then
               maxFixNum = tmp_long + 1
               bestAnswer = ""
            End If
            If bestAnswer = "" Then
               bestAnswer = str2
            Else
               bestAnswer = bestAnswer & "&" & str2
            End If
         End If
      End If
   End If
End If
rs_pub.movenext
Loop
rs_pub.Close
If maxFixNum = 0 Or bestAnswer = "" Then
   our_array = Split(qqchat_table_noanswer, "&")
   qqchat_talk$ = our_array(Int((UBound(our_array) + 1) * Rnd))
   sql = "select * from qqchat_noask "
   rs_pub.open sql, conn, 1, 3
   rs_pub.addnew
   rs_pub("noask") = word$
   rs_pub.Update
   rs_pub.Close
Else
   my_array = Split(bestAnswer, "&")
   qqchat_talk$ = my_array(Int((UBound(my_array) + 1) * Rnd))
End If
End Function

Sub qqchat_domode1()
word$ = Trim(Form1.Text11.Text)
If word$ = "" Then
If Int(Rnd * 9) = 1 Then qqchat_mode = 0
Form1.Text10.Text = ""
Exit Sub
End If

tmp_array = Split(qqchat_table_outstudy, vbCrLf)
For i = 0 To UBound(tmp_array) - 1
    If InStr(1, word$, tmp_array(i)) > 0 And (Len(word$) - Len(tmp_array(i)) <= 4) Then qqchat_mode = 0
Next
If Int(Rnd * rnd_outstudy) = 1 Then qqchat_mode = 0  '一定概率自动退出学习状态
If qqchat_mode = 0 Then
   Form1.Text10.Text = ""
Else
   If qqchat_firstin <> 1 Then
      If lastAskStr <> "" Then
         If Len(lastAskStr) > 3 And Len(word$) > 3 And Right(lastAskStr, 1) <> Left(lastAskStr, 1) Then
            sql = "select * from qqchat_faq "
            rs_pub.open sql, conn, 1, 3
            rs_pub.addnew
            rs_pub("ask") = addkgtostr$(lastAskStr)
            rs_pub("answer") = word$
            rs_pub.Update
            rs_pub.Close
         End If
      End If
   End If
   sql = "select * from qqchat_noask "
   rs_pub.open sql, conn, 1, 3
   If rs_pub.RecordCount > 0 Then
      rs_pub.Move Int(rs_pub.RecordCount * Rnd)
      Form1.Text10.Text = rs_pub("noask")
      rs_pub.Delete
      rs_pub.Update
      rs_pub.Close
   Else
      qqchat_mode = 0
   End If
   If qqchat_mode = 0 Then Form1.Text10.Text = ""
End If
qqchat_firstin = 0
lastAskStr = Form1.Text10.Text
End Sub

Function addkgtostr$(word$)
a = Len(word$)
For i = 1 To a
zifu$ = Mid(word$, i, 1)
zifu$ = zifu$ + " "
thestr$ = thestr$ + zifu$
Next
thestr$ = Trim(thestr$)
addkgtostr$ = thestr$
End Function

Sub save_qqchat_setting()
sql = "update mysetting set [myset1]='" & CStr(Form1.Timer1.Interval) & "|" & CStr(rnd_instudy) & "|" & CStr(rnd_outstudy) & "|" & CStr(Form1.Check4.Value) & "|" & CStr(Form1.Check5.Value) & "' where [id]=2"
conn.execute sql
sql = "update mysetting set [myset1]='" & qqchat_hotkey & "' where [id]=3"
conn.execute sql
End Sub










