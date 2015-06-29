; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CExeDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Exe.h"

ClassCount=3
Class1=CExeApp
Class2=CExeDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_EXE_DIALOG

[CLS:CExeApp]
Type=0
HeaderFile=Exe.h
ImplementationFile=Exe.cpp
Filter=N

[CLS:CExeDlg]
Type=0
HeaderFile=ExeDlg.h
ImplementationFile=ExeDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_LIST1

[CLS:CAboutDlg]
Type=0
HeaderFile=ExeDlg.h
ImplementationFile=ExeDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_EXE_DIALOG]
Type=1
Class=CExeDlg
ControlCount=9
Control1=BTN_CONTROLSOURCE,button,1342242816
Control2=BTN_FREECONTROLSOURCE,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=BTN_DELETELPK,button,1342242816
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=ID_SCANFILENAME,static,1342308352
Control8=IDC_OKLIST,listbox,1353777409
Control9=IDC_FAILLIST,listbox,1353777409

