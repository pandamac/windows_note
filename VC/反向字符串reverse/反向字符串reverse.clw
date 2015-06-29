; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CReverseDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "反向字符串reverse.h"

ClassCount=4
Class1=CReverseApp
Class2=CReverseDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_REVERSE_DIALOG

[CLS:CReverseApp]
Type=0
HeaderFile=反向字符串reverse.h
ImplementationFile=反向字符串reverse.cpp
Filter=N

[CLS:CReverseDlg]
Type=0
HeaderFile=反向字符串reverseDlg.h
ImplementationFile=反向字符串reverseDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=反向字符串reverseDlg.h
ImplementationFile=反向字符串reverseDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_REVERSE_DIALOG]
Type=1
Class=CReverseDlg
ControlCount=5
Control1=IDC_EDIT1,edit,1350631552
Control2=IDC_EDIT2,edit,1350631552
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_BUTTON1,button,1342242816

