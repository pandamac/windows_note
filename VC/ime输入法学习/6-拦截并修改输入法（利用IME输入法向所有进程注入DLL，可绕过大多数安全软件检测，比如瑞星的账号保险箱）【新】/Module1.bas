Attribute VB_Name = "Module1"
Public Declare Function CopyFile Lib "kernel32" Alias "CopyFileA" (ByVal lpExistingFileName As String, ByVal lpNewFileName As String, ByVal bFailIfExists As Long) As Long
Public Declare Function MoveFile Lib "kernel32" Alias "MoveFileA" (ByVal lpExistingFileName As String, ByVal lpNewFileName As String) As Long
Public Declare Function ImmInstallIME Lib "imm32.dll" Alias "ImmInstallIMEA" (ByVal lpszIMEFileName As String, ByVal lpszLayoutText As String) As Long
Public Declare Function UnloadKeyboardLayout Lib "user32" (ByVal HKL As Long) As Long
Public Declare Function GetKeyboardLayoutList Lib "user32" (ByVal nBuff As Long, lpList As Long) As Long
Public Declare Function ImmIsIME Lib "imm32.dll" (ByVal HKL As Long) As Long
Public Declare Function ImmGetDescription Lib "imm32.dll" Alias "ImmGetDescriptionA" (ByVal HKL As Long, ByVal lpsz As String, ByVal uBufLen As Long) As Long
Public Declare Function ImmGetIMEFileName Lib "imm32.dll" Alias "ImmGetIMEFileNameA" (ByVal HKL As Long, ByVal lpStr As String, ByVal uBufLen As Long) As Long
Public Declare Function LoadKeyboardLayout Lib "user32" Alias "LoadKeyboardLayoutA" (ByVal pwszKLID As String, ByVal flags As Long) As Long
Public Declare Function ActivateKeyboardLayout Lib "user32" (ByVal HKL As Long, ByVal flags As Long) As Long
Public Declare Function GetKeyboardLayout Lib "user32" (ByVal dwLayout As Long) As Long
Public Declare Function GetKeyboardLayoutName Lib "user32" Alias "GetKeyboardLayoutNameA" (ByVal pwszKLID As String) As Long
Public Declare Function PostMessage Lib "user32" Alias "PostMessageA" (ByVal hWnd As Long, ByVal wMsg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Public Declare Function FindWindowEx Lib "user32" Alias "FindWindowExA" (ByVal hWnd1 As Long, ByVal hWnd2 As Long, ByVal lpsz1 As String, ByVal lpsz2 As String) As Long
Public Declare Function SystemParametersInfo Lib "user32" Alias "SystemParametersInfoA" (ByVal uAction As Long, ByVal uParam As Long, lpvParam As Any, ByVal fuWinIni As Long) As Long
Public Declare Function RegisterWindowMessage Lib "user32" Alias "RegisterWindowMessageA" (ByVal lpString As String) As Long
Public Declare Function SetWindowLong Lib "user32" Alias "SetWindowLongA" (ByVal hWnd As Long, ByVal nIndex As Long, ByVal dwNewLong As Long) As Long
Public Declare Function CallWindowProc Lib "user32" Alias "CallWindowProcA" (ByVal lpPrevWndFunc As Long, ByVal hWnd As Long, ByVal Msg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Public Declare Function RegOpenKey Lib "advapi32.dll" Alias "RegOpenKeyA" (ByVal hKey As Long, ByVal lpSubKey As String, phkResult As Long) As Long
Public Declare Function RegEnumValue Lib "advapi32.dll" Alias "RegEnumValueA" (ByVal hKey As Long, ByVal dwIndex As Long, ByVal lpValueName As String, lpcbValueName As Long, ByVal lpReserved As Long, lpType As Long, lpData As Any, lpcbData As Long) As Long
Public Declare Function RegCloseKey Lib "advapi32.dll" (ByVal hKey As Long) As Long
Public Declare Function RegDeleteValue Lib "advapi32.dll" Alias "RegDeleteValueA" (ByVal hKey As Long, ByVal lpValueName As String) As Long
' -------------------------Windows API-----------------------------------------

Public Declare Function IMESetPubString Lib "imedllhost09.ime" (ByVal RunDLLStr As String, ByVal UnloadDll As Long, ByVal loadNextIme As Long, ByVal DllData1 As Long, ByVal DllData2 As Long, ByVal DllData3 As Long) As Long
Public Declare Function IMEClearPubString Lib "imedllhost09.ime" () As Long
' -----------------------输入法扩展服务导出函数--------------------------------

Public Declare Function DLLstartHOOK Lib "hxwdllwx.dll" (ByVal hWnd As Long) As Long   '初始化输入法HOOK
'Public Declare Function DLLstopHOOK Lib "hxwdllwx.dll" (ByVal a As Long, ByVal b As Long) As Long    '（空函数）
Public Declare Function DLLsetHOOKState Lib "hxwdllwx.dll" (ByVal myState As Boolean) As Long  '打开或关闭钩子
Public Declare Function DLLGetPubString Lib "hxwdllwx.dll" () As String   '获得输入法输入
Public Declare Function DLLSetPubString Lib "hxwdllwx.dll" (ByVal tmpStr As String) As Long   '修改输入法输入
' -----------------------------------------目标DLL导出函数-----------------------


Public Const KLF_REORDER = &H8
Public Const WM_INPUTLANGCHANGEREQUEST = &H50
Public Const SPI_SETDEFAULTINPUTLANG = 90
Public Const SPI_GETDEFAULTINPUTLANG = 89
Public Const SPIF_UPDATEINIFILE = &H1
Public Const SPIF_SENDWININICHANGE = &H2
Public Const GWL_WNDPROC = -4
Public Const HKEY_CURRENT_USER = &H80000001
Public Const ERROR_NO_MORE_ITEMS = 259&

Public WM_HXWDLLWX_QQBTX As Long  '自定义消息
Public PrevWndProc As Long '保存旧的窗口处理函数地址
Public GSystemPath As String    '系统路径
Public GPathStr As String   '当前路径
Public GImeFileName As String   '输入法文件名，不含扩展名
Public GSysDefIme As Long  '系统默认输入法的句柄
Public CilentDllPath As String
Public ImeHKL As Long   '输入法句柄
Public ImeHKLString As String   '输入法句柄字符串

Public Function SubWndProc(ByVal hWnd As Long, ByVal Msg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Dim tmpS As String
If Msg = WM_HXWDLLWX_QQBTX Then
'如果收到了输入法拦截消息
    tmpS = DLLGetPubString() '获得输入法输入
    Form1.Text1.Text = Form1.Text1.Text & "拦截到输入法输入：" & tmpS & vbCrLf
    tmpS = tmpS & "（被修改）"
    DLLSetPubString tmpS   '修改输入法输入
End If
SubWndProc = CallWindowProc(PrevWndProc, hWnd, Msg, wParam, lParam)   '将消息传给旧的窗口函数继续处理
End Function


Public Function StringVC2VB(str As String) As String
StringVC2VB = ""
If Len(str) = 0 Then Exit Function
Dim SPE0 As Long
SPE0 = InStr(str, Chr(0))
If SPE0 Then
    StringVC2VB = Left(str, SPE0 - 1)
Else
    StringVC2VB = str
End If
End Function

Public Function MSetUpIME(ByVal lpFile As String) As Long
' 安装输入法，成功则返回输入法句柄。参数为输入法文件名，不含扩展名
'一定要将文件复制到系统目录，并且当前目录要保留一份同名文件，或者要先将工作路径切换到系统目录下，这样输入法才能安装成功
Dim tmpCDir As String, tmpDirArray() As String
tmpCDir = CurDir()
tmpDirArray = Split(GSystemPath, ":")
If UBound(tmpDirArray) >= 0 Then ChDrive tmpDirArray(0) & ":"
' --------------------
'MoveFile GPathStr & lpFile & ".dll", GPathStr & lpFile & ".ime"  '将输入法文件重命名，扩展名必须是IME
'CopyFile GPathStr & lpFile & ".ime", GSystemPath & lpFile & ".ime", False   '将输入法文件复制到系统目录
CopyFile GPathStr & lpFile & ".dll", GSystemPath & lpFile & ".ime", False   '将输入法文件复制到系统目录
ChDir GSystemPath  '安装前一定要将工作目录切换到系统目录下，否则无法安装成功
MSetUpIME = ImmInstallIME(lpFile & ".ime", "Windows标准输入法扩展服务 v1.0")
'MoveFile GPathStr & lpFile & ".ime", GPathStr & lpFile & ".dll"  '安装完成后再将名字改回来
' --------------------
tmpDirArray = Split(tmpCDir, ":")
If UBound(tmpDirArray) >= 0 Then ChDrive tmpDirArray(0) & ":"
ChDir tmpCDir    '安装完成后再将工作目录切换回来
End Function

Public Function MGetIMEHwndString(ByVal IMEhwnd As Long) As String
' 返回输入法句柄字符串
Dim tempIM As Long, retV As Long, HKLName As String
MGetIMEHwndString = ""
tempIM = GetKeyboardLayout(0)
'ActivateKeyboardLayout IMEhwnd, KLF_REORDER    '指定输入法设置为列表第一项
ActivateKeyboardLayout IMEhwnd, 0
HKLName = String(8, 0)
retV = GetKeyboardLayoutName(HKLName)
ActivateKeyboardLayout tempIM, 0
If HKLName <> String(8, 0) Then MGetIMEHwndString = HKLName
End Function

Public Sub MSetIMEIntoFirst(ByVal HKLName As String)
' 将指定句柄的输入法设置为输入法列表的第一项
retV = LoadKeyboardLayout(HKLName, KLF_REORDER)  '指定输入法设置为列表第一项
End Sub

Public Sub MSetIMEIsDefInput(ByVal IMEhwnd As Long)
' 设置指定句柄的输入法为系统默认输入法，一开机就加载
Dim retV As Long
retV = SystemParametersInfo(SPI_SETDEFAULTINPUTLANG, 0, IMEhwnd, SPIF_SENDWININICHANGE)
End Sub

Public Sub MQuitIMEIsDefInput(ByVal IMEhwnd As Long)
'还原系统默认输入法。用于程序退出时还原对默认键盘布局的改变
Dim retV As Long
SystemParametersInfo SPI_GETDEFAULTINPUTLANG, 0, retV, 0 '得到系统默认的输入法的句柄
If retV <> IMEhwnd Then MSetIMEIsDefInput IMEhwnd
End Sub

Public Function MUnLoadIMEByHKL(ByVal IMEhwnd As Long) As Long
'通过输入法句柄卸载输入法
MUnLoadIMEByHKL = UnloadKeyboardLayout(IMEhwnd)
End Function

Public Function MUnLoadIMEByName(ByVal lpFile As String, Optional ByVal lpText As String = "Windows标准输入法扩展服务") As Long
'通过输入法文件名称卸载输入法
Dim retV As Long, hIme As Long
hIme = MImeFindByName(lpFile, lpText)
If hIme <> 0 Then
    retV = UnloadKeyboardLayout(hIme)
End If
MUnLoadIMEByName = retV
End Function

Public Function MImeFindByName(ByVal lpFile As String, Optional ByVal lpText As String = "Windows标准输入法扩展服务") As Long
'通过文件名称查找已安装的输入法句柄。lpFile=文件名，lpText=输入法名称
Dim IMEhKB() As Long, IMEBuffLen As Long, i As Long, retV As Long
Dim IMEBuff As String, iCount As Long
Dim IMERetStr As String
Dim IMERetCount As Long
MImeFindByName = 0
iCount = GetKeyboardLayoutList(0, ByVal 0) '取得输入法数量
If iCount < 1 Then Exit Function
ReDim IMEhKB(iCount - 1)
retV = 0
iCount = GetKeyboardLayoutList(UBound(IMEhKB) + 1, IMEhKB(0)) '取得所有输入法
For i = 1 To iCount
    If ImmIsIME(IMEhKB(i - 1)) = 1 Then '如果是中文输入法
        IMEBuffLen = 255
        IMEBuff = String(255, 0)
        IMERetCount = ImmGetDescription(IMEhKB(i - 1), IMEBuff, IMEBuffLen)
        IMERetStr = Left(IMEBuff, IMERetCount)
        IMERetStr = StringVC2VB(IMERetStr)
        If IMERetStr = lpText Then
            ' 判断输入法文件
            IMEBuffLen = 255
            IMEBuff = String(255, 0)
            IMERetCount = ImmGetIMEFileName(IMEhKB(i - 1), IMEBuff, IMEBuffLen)
            IMERetStr = Left(IMEBuff, IMERetCount)
            IMERetStr = StringVC2VB(IMERetStr)
            If UCase(IMERetStr) = UCase(lpFile & ".ime") Then
                retV = IMEhKB(i - 1)
                Exit For
            End If
        End If
    End If
Next
MImeFindByName = retV
End Function


Sub MActiveIMEForWindows(ByVal IMEhwnd As Long, mType As Byte)
' 在所有已经运行的程序中激活指定输入法，IMEhwnd=输入法句柄。mType=激活强度，0-只尝试顶级窗口，1-尝试所有子窗口
If mType = 0 Then
    PostMSG2TopWindow IMEhwnd
Else
    PostMSG2AllWindow IMEhwnd
End If
End Sub

Sub PostMSG2TopWindow(ByVal IMEhwnd As Long)
' 在所有的顶级窗口中激活指定句柄的输入法
Dim mHwnd As Long
mHwnd = 0
Do
    mHwnd = FindWindowEx(0, mHwnd, vbNullString, vbNullString)    '遍历桌面顶级窗口
    If mHwnd <> 0 Then
        PostMessage mHwnd, WM_INPUTLANGCHANGEREQUEST, &H1, IMEhwnd   '在目标窗口中激活指定输入法
    End If
Loop Until mHwnd = 0
End Sub

Sub PostMSG2AllWindow(ByVal IMEhwnd As Long)
' 在所有的顶级窗口和其下级窗口中激活指定句柄的输入法
' 遍历指定窗口mHwnd的所有子窗口,并在其中激活指定输入法
Dim theLPS() As Long, tmpHwnd As Long, mHwnd As Long
mHwnd = 0 '欲遍历的顶级窗口句柄，0代表桌面
ReDim theLPS(1)
theLPS(1) = mHwnd
tmpHwnd = 0
Do   '遍历主循环
    tmpHwnd = FindWindowEx(theLPS(UBound(theLPS)), tmpHwnd, vbNullString, vbNullString)
    If tmpHwnd <> 0 Then
        PostMessage tmpHwnd, WM_INPUTLANGCHANGEREQUEST, &H1, IMEhwnd   '发送消息，激活输入法
        ReDim Preserve theLPS(UBound(theLPS) + 1)
        theLPS(UBound(theLPS)) = tmpHwnd
        tmpHwnd = 0
    Else
        tmpHwnd = theLPS(UBound(theLPS))
        ReDim Preserve theLPS(UBound(theLPS) - 1)
    End If
Loop Until UBound(theLPS) < 1
End Sub

Sub MDeleteRegIme(ByVal ImeReg As String)
' 删除注册表中该输入法的相关项
Dim hKey As Long, sName As String, sData As String, sRet As Long, RetData As Long, sCnt As Long
If RegOpenKey(HKEY_CURRENT_USER, "Keyboard Layout\Preload", hKey) = 0 Then
    sName = Space(255)
    sData = Space(255)
    sRet = 255
    RetData = 255
    Do While RegEnumValue(hKey, sCnt, sName, sRet, 0, ByVal 0&, ByVal sData, RetData) <> ERROR_NO_MORE_ITEMS
        If RetData > 0 Then
            If UCase(Left(sData, RetData - 1)) = UCase(ImeReg) Then
                RegDeleteValue hKey, Left(sName, sRet)    '移除输入法对应的注册表值
                Exit Do
            End If
            sCnt = sCnt + 1
            sName = Space(255)
            sData = Space(255)
            sRet = 255
            RetData = 255
        End If
    Loop
    RegCloseKey hKey
End If
End Sub
