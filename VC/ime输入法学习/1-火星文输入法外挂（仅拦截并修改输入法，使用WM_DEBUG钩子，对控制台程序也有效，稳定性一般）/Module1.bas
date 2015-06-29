Attribute VB_Name = "Module1"
Public web_cachename_myword As String
Public web_cachename_fanti As String
Public web_cachename_yiti As String
Public web_cachename_mysame As String
Public web_cachename_seeout As String
Public web_cachename_gailv As String
Public web_cachename_myworde As String
Public web_cachename_sameworde As String
Public web_cachename_mywords As String
Public web_cachename_samewords As String
Public web_cachename_mychar As String
Public web_cachename_mychars As String

Public web_char_wordtable() As String
Public web_table_pingyin As String
' -----------------------------------转换表变量-------------------
Public conn As Object, rs_pub As Object
Public set_work_mode As Integer, set_combo1 As String, set_bt2bt As Integer, set_jt2bt_type As Integer
Public set_jt2bt_t1 As Double, set_jt2bt_t2 As Integer, set_jt2bt_t3 As Integer, set_jt2bt_t4 As Integer, set_jt2bt_t5 As Integer
Public set_bt2bt_t1 As Integer, set_bt2bt_t2 As Integer, set_bt2bt_t3 As String

' -----------------------------------其它全局变量-----------------


Sub init_table()
' -------------------------------------------------
Set rs = CreateObject("adodb.recordset")
sql = "select * from myqqbtz"
rs.open sql, conn, 1, 1
web_cachename_myword = rs("mytext")
rs.movenext
web_cachename_fanti = rs("mytext")
rs.movenext
web_cachename_yiti = rs("mytext")
rs.movenext
web_cachename_mysame = rs("mytext")
rs.movenext
web_cachename_seeout = rs("mytext")
rs.movenext
web_cachename_gailv = rs("mytext")
rs.movenext
web_cachename_myworde = rs("mytext")
rs.movenext
web_cachename_sameworde = rs("mytext")
rs.movenext
web_cachename_mywords = rs("mytext")
rs.movenext
web_cachename_samewords = rs("mytext")
rs.movenext
web_cachename_mychar = rs("mytext")
rs.Close
Set rs = Nothing
' -------------------------------------------------
    Set cache_rs_lei = CreateObject("adodb.recordset")
      sql = "select * from mychars"
      cache_rs_lei.open sql, conn, 1, 1
      cache_tempstr1 = ""
        Do While Not cache_rs_lei.EOF
              cache_tempstr1 = cache_tempstr1 & cache_rs_lei("myname") & "," & cache_rs_lei("mydata") & vbCrLf
              cache_rs_lei.movenext
        Loop
      cache_rs_lei.Close
      Set cache_rs_lei = Nothing
  web_cachename_mychars = cache_tempstr1
 ' -------------------------------------------------
 Set rs = CreateObject("adodb.recordset")
 sql = "select * from bihua_py"
 rs.open sql, conn, 1, 1
 web_char_wordtable = Split(rs("bihua_data"), vbCrLf)
 
 rs.movenext
 web_table_pingyin = rs("bihua_data")
 rs.Close
 Set rs = Nothing
 ' ------------------------
End Sub



Function jt2bt(myword, mypinglv, chihui, changyong, seeout, ifwordsame)
If CDbl(mypinglv) = 0 Then
   jt2bt = myword
   Exit Function
End If
word_long = Len(myword)
Dim table_mubiao()
ReDim table_mubiao(word_long - 1)     '转换目标表
For i = 0 To word_long - 1
   table_mubiao(i) = Mid(myword, i + 1, 1)
Next
table_dongjie = String(word_long, "0") '冻结标志，被冻结的字符不再参与转换
table_gailv = ""                      ' 字符常用程度表

If changyong = "1" Then        '建立字符常用程度表table_gailv
mype = 0
   For i = 1 To word_long
       mype = InStr(1, web_cachename_myword, Mid(myword, i, 1)) '建立常用字符概率表
       If mype = 0 Then
         table_gailv = table_gailv & "9"
       Else
         table_gailv = table_gailv & Mid(web_cachename_gailv, mype, 1)
       End If
   Next
Else
   For i = 1 To word_long
       table_gailv = table_gailv & CStr(Int(10 * Rnd))
   Next
End If


If chihui = "1" Then
web_table_mywords = Split(web_cachename_mywords, vbCrLf)
web_table_samewords = Split(web_cachename_samewords, vbCrLf)
temp_con = UBound(web_table_mywords)
mype = 0
  For i = 0 To temp_con
  If web_table_mywords(i) <> "" Then
     temp_start = 1
     words_samewords = Split(web_table_samewords(i), ",")
     Do
     mype = InStr(temp_start, myword, web_table_mywords(i))
     If mype <> 0 Then
        temp_start = mype + Len(web_table_mywords(i))
        temp_falg = 0
        For i2 = mype To mype + Len(web_table_mywords(i)) - 1 '检查字符是否已被冻结
            If Mid(table_dongjie, i2, 1) = "1" Then temp_falg = 1
        Next
        If temp_falg = 0 Then      '没有冻结，开始转换
            words_samewords_chan = words_samewords(Int((UBound(words_samewords) + 1) * Rnd))
            For i3 = mype - 1 To mype - 1 + Len(web_table_mywords(i)) - 1
                If i3 = mype - 1 Then
                   table_mubiao(i3) = words_samewords_chan
                Else
                   table_mubiao(i3) = ""
                End If
                Mid(table_dongjie, i3 + 1, 1) = "1"     '将转换后的字符冻结，避免重复转换
            Next
        End If
     End If
     Loop Until mype = 0
  End If
  Next
End If



web_table_sameworde = Split(web_cachename_sameworde, vbCrLf)
For i = 1 To word_long
    If Mid(table_dongjie, i, 1) = "0" Then '检查字符是否已被冻结
       mywordtmp = Mid(myword, i, 1)
       mype = InStr(1, web_cachename_myworde, mywordtmp)
       If mype <> 0 Then
          english_array = Split(web_table_sameworde(mype - 1), ",")
          table_mubiao(i - 1) = english_array(Int((UBound(english_array) + 1) * Rnd))
          Mid(table_dongjie, i, 1) = "1" '将转换后的字符冻结，避免重复转换
       End If
    End If
Next


'-----------------------------------------------------
should_word = Int(word_long * CDbl(mypinglv) + 0.5) '多少字符应该被转换
If should_word > word_long Then should_word = word_long
If should_word <= 0 Then should_word = 1
have_word = 0
For i = 1 To word_long
If Mid(table_dongjie, i, 1) = "1" Then have_word = have_word + 1
Next
should_word = should_word - have_word
If should_word >= 1 Then
 Dim gailv_word_now(), gailv_word_last()
 ReDim gailv_word_now(word_long - 1)
 ReDim gailv_word_last(word_long - 1)
 For i = 0 To word_long - 1
    gailv_word_now(i) = i + 1   '概率排行表
    gailv_word_last(i) = CInt(Mid(table_gailv, i + 1, 1))
 Next
 For i = 0 To word_long - 1
     If i > 0 Then
        If gailv_word_last(i) > gailv_word_last(i - 1) Then
           temp_n = gailv_word_last(i)
           gailv_word_last(i) = gailv_word_last(i - 1)
           gailv_word_last(i - 1) = temp_n
           temp_m = gailv_word_now(i)
           gailv_word_now(i) = gailv_word_now(i - 1)
           gailv_word_now(i - 1) = temp_m
           i = i - 2
        End If
     End If
 Next
 
   table_bigsame = Split(web_cachename_mysame, vbCrLf)
   table_bigsame_diff = Split(web_cachename_seeout, vbCrLf)
   con_tmp = 0
   For i = 1 To should_word
       If Mid(table_dongjie, gailv_word_now(con_tmp), 1) = "0" Then
          tike_word = Mid(myword, gailv_word_now(con_tmp), 1)
          mype = InStr(1, web_cachename_myword, tike_word)
          If mype = 0 Then
             i = i - 1
          Else
             table_bigsame_wordkey = table_bigsame(mype - 1)
             If table_bigsame_wordkey = "" Then
              i = i - 1
             Else
             table_bigsame_word = Split(table_bigsame_wordkey, ",")
             table_bigsame_difficu = table_bigsame_diff(mype - 1)
             If table_bigsame_difficu = "" Then
               table_bigsame_difficu = String(UBound(table_bigsame_word), ",")
               If table_bigsame_difficu = "" Then table_bigsame_difficu = "1"
             End If
             table_bigsame_di = Split(table_bigsame_difficu, ",")
             Dim paixu()
             ReDim paixu(UBound(table_bigsame_word))
             fuhe = 0
             For tmp_paixu = 0 To UBound(table_bigsame_word)
                 If table_bigsame_di(tmp_paixu) = "" Then table_bigsame_di(tmp_paixu) = "1"
                 If CInt(seeout) >= CInt(table_bigsame_di(tmp_paixu)) Then
                   paixu(fuhe) = tmp_paixu
                   fuhe = fuhe + 1
                 End If
             Next
             If fuhe = 0 Then
                i = i - 1
             Else
                fuhe = Int(Rnd * fuhe)
                If ifwordsame = "1" Then
                   For tm_m = 1 To word_long
                       If Mid(myword, tm_m, 1) = tike_word And Mid(table_dongjie, tm_m, 1) = "0" Then
                          table_mubiao(tm_m - 1) = table_bigsame_word(paixu(fuhe))
                          Mid(table_dongjie, tm_m, 1) = "1" '冻结
                       End If
                   Next
                Else
                   table_mubiao(gailv_word_now(con_tmp) - 1) = table_bigsame_word(paixu(fuhe))
                   Mid(table_dongjie, gailv_word_now(con_tmp), 1) = "1" '冻结
                End If
             End If
             End If
          End If
       Else
       i = i - 1
       End If
       con_tmp = con_tmp + 1
       If con_tmp > word_long - 1 Then
         Exit For
       End If
   Next
End If

myallend = ""
For i = 0 To word_long - 1
   myallend = myallend & table_mubiao(i)
Next

jt2bt = myallend
End Function

Function bt2jt(myword, mytype)
If mytype = "1" Or mytype = "3" Then
web_table_mywords = Split(web_cachename_mywords, vbCrLf)
web_table_samewords = Split(web_cachename_samewords, vbCrLf)
web_table_samewords_long = UBound(web_table_samewords)
For i = 0 To web_table_samewords_long
   words_samewords = Split(web_table_samewords(i), ",")
   words_samewords_long = UBound(words_samewords)
   For n = 0 To words_samewords_long
      myword = Replace(myword, words_samewords(n), web_table_mywords(i))
   Next
Next
End If


If mytype = "1" Or mytype = "2" Then
   table_bigsame = Split(web_cachename_mysame, vbCrLf)
   table_bigsame_long = UBound(table_bigsame)
   For i = 0 To table_bigsame_long
      word_oneword = Mid(web_cachename_myword, i + 1, 1)
      bigsame_sameword = Split(table_bigsame(i), ",")
      bigsame_sameword_long = UBound(bigsame_sameword)
      For n = 0 To bigsame_sameword_long
         myword = Replace(myword, bigsame_sameword(n), word_oneword)
      Next
   Next
End If


temp_table = web_cachename_mychar
temp_i = Len(temp_table)
For i = 1 To temp_i
myword = Replace(myword, Mid(temp_table, i, 1), "")
Next

bt2jt = myword
End Function

Function fun_myaddcharran(myword, mymidu)
word_long = Len(myword)
char_long = Len(web_cachename_mychar)
should_word = Int(word_long * (CDbl(mymidu) / 100) + 0.5)
If should_word < 1 Then should_word = 1
For i = 1 To should_word
    tike_char = Mid(web_cachename_mychar, Int(char_long * Rnd) + 1, 1)
    pick = Int((Len(myword) + 1) * Rnd)
    myword = Left(myword, pick) & tike_char & Right(myword, Len(myword) - pick)
Next
fun_myaddcharran = myword
End Function

Function fun_myaddcharding(myword, mybianhao)
table_guding = Split(web_cachename_mychars, vbCrLf)
For i = 0 To UBound(table_guding)
If table_guding(i) <> "" Then
table_pick = Split(table_guding(i), ",")
If table_pick(0) = mybianhao Then
   If table_pick(1) <> "" Then
      myword2 = table_pick(1)
      For m = 1 To Len(myword)
          myword2 = myword2 & Mid(myword, m, 1) & table_pick(1)
      Next
      myword = myword2
   Else
      middle = Int(Len(myword) / 2)
      word1 = Left(myword, middle)
      word2 = Right(myword, Len(myword) - middle)
      myword = table_pick(2) & word1 & table_pick(3) & word2 & table_pick(4)
   End If
Exit For
End If
End If
Next
fun_myaddcharding = myword
End Function

Function bt2bt(myword, mytype, mymidu, mybianhao)
myoutput = ""
If mytype = "随机" Then
   If IsNumeric(mymidu) = False Then mymidu = "20"
   myoutput = fun_myaddcharran(myword, mymidu)
End If

If mytype = "固定" Then
   myoutput = fun_myaddcharding(myword, mybianhao)
End If
bt2bt = myoutput
End Function

Function jt2ft(myword, mytype)
myconst = Len(myword)
myword2 = ""
mywordtmp = ""
mywordtmp2 = ""
mype = 0

If mytype = "简体转繁体" Then
For i = 1 To myconst
   mywordtmp = Mid(myword, i, 1)
   mype = InStr(1, web_cachename_myword, mywordtmp)
   If mype = 0 Then
      myword2 = myword2 & mywordtmp
   Else
      mywordtmp2 = Mid(web_cachename_fanti, mype, 1)
      If mywordtmp2 = "*" Then
         myword2 = myword2 & mywordtmp
      Else
         myword2 = myword2 & mywordtmp2
      End If
   End If
Next
End If

If mytype = "简体转异体" Then
For i = 1 To myconst
   mywordtmp = Mid(myword, i, 1)
   mype = InStr(1, web_cachename_myword, mywordtmp)
   If mype = 0 Then
      myword2 = myword2 & mywordtmp
   Else
      mywordtmp2 = Mid(web_cachename_yiti, mype, 1)
      If mywordtmp2 = "*" Then
         myword2 = myword2 & mywordtmp
      Else
         myword2 = myword2 & mywordtmp2
      End If
   End If
Next
End If

If mytype = "繁体转简体" Then
For i = 1 To myconst
   mywordtmp = Mid(myword, i, 1)
   If mywordtmp = "*" Then
      mype = 0
   Else
      mype = InStr(1, web_cachename_fanti, mywordtmp)
   End If
   If mype = 0 Then
      myword2 = myword2 & mywordtmp
   Else
      mywordtmp2 = Mid(web_cachename_myword, mype, 1)
      myword2 = myword2 & mywordtmp2
   End If
Next
End If

jt2ft = myword2
End Function

Public Function pub_fun_bt2bt(ByVal myinput As String) As String
Dim mytype As String
If set_bt2bt_t1 = 0 Then mytype = "随机" Else mytype = "固定"
pub_fun_bt2bt = bt2bt(myinput, mytype, set_bt2bt_t2, set_bt2bt_t3)
End Function

Public Function pub_fun_jt2bt(ByVal myinput As String) As String
Dim t1 As String, t2 As String, t3 As String, t4 As String, t5 As String
If set_combo1 = "简体转火星文" Then
   If set_jt2bt_type = 0 Then
      t1 = "1"
      t2 = "0"
      t3 = "0"
      t4 = "3"
      t5 = "0"
   End If
   If set_jt2bt_type = 1 Then
      t1 = "1"
      t2 = "1"
      t3 = "1"
      t4 = "3"
      t5 = "0"
   End If
   If set_jt2bt_type = 2 Then
      t1 = "0.5"
      t2 = "0"
      t3 = "1"
      t4 = "3"
      t5 = "1"
   End If
   If set_jt2bt_type = 3 Then
      t1 = CStr(set_jt2bt_t1)
      t2 = CStr(set_jt2bt_t2)
      t3 = CStr(set_jt2bt_t3)
      t4 = CStr(set_jt2bt_t4 + 1)
      t5 = CStr(set_jt2bt_t5)
   End If
   pub_fun_jt2bt = jt2bt(myinput, t1, t2, t3, t4, t5)
Else
   pub_fun_jt2bt = jt2ft(myinput, set_combo1)
End If
If set_bt2bt = 1 Then pub_fun_jt2bt = pub_fun_bt2bt(pub_fun_jt2bt)
End Function


Function char_wordnum(my_char_word)
Dim wordnum_i, wordnum_j, wordnum_k, temp_wordnum
temp_wordnum = ""
If my_char_word = "" Then
char_wordnum = 0
Exit Function
End If
wordnum_i = 0
wordnum_j = 0
wordnum_k = 0
For wordnum_i = 1 To Len(my_char_word)
  temp_wordnum = Mid(my_char_word, wordnum_i, 1)
  For wordnum_j = 1 To 26
     If InStr(1, web_char_wordtable(wordnum_j), temp_wordnum) > 0 Then
       wordnum_k = wordnum_k + wordnum_j
       Exit For
     End If
  Next
Next
char_wordnum = wordnum_k
End Function


Function PinYinGetByText(pText)
 Dim table_pingyin() As String
 ReDim table_pingyin(65535)
  For tIndex = 0 To 65535
    table_pingyin(tIndex) = Chr(tIndex - 32768)
  Next

  tStrings = Split(web_table_pingyin, ";")
  tStrings_Length = UBound(tStrings)

  For tIndex = 0 To tStrings_Length
    tPY_DataStrings = Split(tStrings(tIndex), ":")
    tPY_DataStrings_Length = UBound(tPY_DataStrings)
    If CBool(tPY_DataStrings_Length) Then
      tPY_Word = tPY_DataStrings(0)
      tPY_Value = tPY_DataStrings(1)
      tPY_Word_Ascii = Asc(tPY_Word) + 32768
      table_pingyin(tPY_Word_Ascii) = tPY_Value
    End If
  Next
  
  tTextCount = Len(pText)
  tCacheSize = Sqr(tTextCount) + 1

  For tIndex = 1 To tTextCount
    tWord = Mid(pText, tIndex, 1)
    tPinYin = "" & Replace(PinYinGetByWord(tWord, table_pingyin), " ", "/") & ""

    tCacheText = tCacheText & " " & tPinYin

    If Not CBool(tIndex Mod tCacheSize) Then
      tOutText = tOutText & tCacheText
      tCacheText = ""
    End If
  Next
  tOutText = tOutText & tCacheText
  tCacheText = ""
  PinYinGetByText = tOutText
End Function

Function PinYinGetByWord(pWord, ByRef table_pingyin() As String)
  tWordAscii = Asc(pWord) + 32768
  PinYinGetByWord = table_pingyin(tWordAscii)
End Function









