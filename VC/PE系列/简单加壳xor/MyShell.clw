; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMyShellDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "MyShell.h"

ClassCount=4
Class1=CMyShellApp
Class2=CMyShellDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_MYSHELL_DIALOG

[CLS:CMyShellApp]
Type=0
HeaderFile=MyShell.h
ImplementationFile=MyShell.cpp
Filter=N

[CLS:CMyShellDlg]
Type=0
HeaderFile=MyShellDlg.h
ImplementationFile=MyShellDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=MyShellDlg.h
ImplementationFile=MyShellDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_MYSHELL_DIALOG]
Type=1
Class=CMyShellDlg
ControlCount=3
Control1=IDC_STATIC,static,1342308352
Control2=IDC_EDIT1,edit,1350631552
Control3=IDC_BTN_SHELL,button,1342242816

