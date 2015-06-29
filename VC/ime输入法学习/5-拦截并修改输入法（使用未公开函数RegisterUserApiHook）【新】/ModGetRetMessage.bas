Attribute VB_Name = "ModGetRetMessage"
Option Explicit
'*************************************************************************
'**模 块 名：ModGetRetMessage
'**说    明：根据API返回值,输出相应的字符串说明.方便API调试.
'**创 建 人：嗷嗷叫的老马
'**日    期：2009年4月16日
'**备    注: 紫水晶工作室 版权所有
'**          更多模块/类模块请访问我站:  http://www.m5home.com
'**版    本：V1.0
'*************************************************************************

Private Declare Function FormatMessage Lib "kernel32.dll" Alias "FormatMessageA" ( _
     ByVal dwFlags As Long, _
     ByRef lpSource As Any, _
     ByVal dwMessageId As Long, _
     ByVal dwLanguageId As Long, _
     ByVal lpBuffer As String, _
     ByVal nSize As Long, _
     ByRef Arguments As Long) As Long

'Private Const FORMAT_MESSAGE_FROM_HMODULE As Long = &H800
'Private Const FORMAT_MESSAGE_FROM_STRING As Long = &H400
'Private Const FORMAT_MESSAGE_FROM_SYSTEM As Long = &H1000

Public Enum MESSAGE_TYPE            '一般只用到这三个值.有更多需要就自己加吧.
    FORMAT_MESSAGE_FROM_HMODULE = &H800
    FORMAT_MESSAGE_FROM_STRING = &H400
    FORMAT_MESSAGE_FROM_SYSTEM = &H1000
End Enum

Public Function GetRetMessage(ByVal lRetValue As Long, ByVal lType As MESSAGE_TYPE) As String
    '根据API返回值,输出相应的字符串说明.
    'lRetValue - API的返回值
    'lType - 按什么类型的消息输出字符串
    '返回值:
    '       成功返回得出的字符串,失败返回空字符串.
    Dim lRet As Long, sBuffer As String
    
    sBuffer = Space(255)
    lRet = FormatMessage(lType, ByVal 0&, lRetValue, 0, sBuffer, 255, ByVal 0&)
    GetRetMessage = Mid(sBuffer, 1, lRet)
End Function
