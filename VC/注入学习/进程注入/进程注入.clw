; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=Csome
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "进程注入.h"

ClassCount=5
Class1=CMyApp
Class2=CMyDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class4=CCommand
Class5=Csome
Resource3=IDD_MY_DIALOG

[CLS:CMyApp]
Type=0
HeaderFile=进程注入.h
ImplementationFile=进程注入.cpp
Filter=N

[CLS:CMyDlg]
Type=0
HeaderFile=进程注入Dlg.h
ImplementationFile=进程注入Dlg.cpp
Filter=D
LastObject=CMyDlg
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=进程注入Dlg.h
ImplementationFile=进程注入Dlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_MY_DIALOG]
Type=1
Class=CMyDlg
ControlCount=6
Control1=IDC_LIST1,SysListView32,1350631425
Control2=IDC_BUTTON1,button,1342242816
Control3=IDC_EDIT1,edit,1350631552
Control4=IDC_EDIT2,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352

[CLS:CCommand]
Type=0
HeaderFile=Command.h
ImplementationFile=Command.cpp
BaseClass=CDialog
Filter=D

[CLS:Csome]
Type=0
HeaderFile=some.h
ImplementationFile=some.cpp
BaseClass=CDialog
Filter=D

