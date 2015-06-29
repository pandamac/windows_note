; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMBRDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "MBR.h"

ClassCount=4
Class1=CMBRApp
Class2=CMBRDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_MBR_DIALOG

[CLS:CMBRApp]
Type=0
HeaderFile=MBR.h
ImplementationFile=MBR.cpp
Filter=N

[CLS:CMBRDlg]
Type=0
HeaderFile=MBRDlg.h
ImplementationFile=MBRDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=MBRDlg.h
ImplementationFile=MBRDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_MBR_DIALOG]
Type=1
Class=CMBRDlg
ControlCount=1
Control1=IDC_BUTTON1,button,1342242816

