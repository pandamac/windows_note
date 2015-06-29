; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CHTTPTestDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "HTTPTest.h"

ClassCount=4
Class1=CHTTPTestApp
Class2=CHTTPTestDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_HTTPTEST_DIALOG

[CLS:CHTTPTestApp]
Type=0
HeaderFile=HTTPTest.h
ImplementationFile=HTTPTest.cpp
Filter=N

[CLS:CHTTPTestDlg]
Type=0
HeaderFile=HTTPTestDlg.h
ImplementationFile=HTTPTestDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=HTTPTestDlg.h
ImplementationFile=HTTPTestDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_HTTPTEST_DIALOG]
Type=1
Class=CHTTPTestDlg
ControlCount=9
Control1=IDC_BUTTON1,button,1342242817
Control2=IDC_EDIT_IP,edit,1350631552
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT_ADDR,edit,1484849280
Control5=IDC_STATIC,static,1342308353
Control6=IDC_STATIC,static,1342308353
Control7=IDC_EDIT_MYIP,edit,1484849280
Control8=IDC_BUTTON_NEW,button,1342242816
Control9=IDC_STATIC,button,1342177287

