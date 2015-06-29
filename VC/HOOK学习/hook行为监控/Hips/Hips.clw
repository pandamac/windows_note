; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CHipsDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Hips.h"

ClassCount=4
Class1=CHipsApp
Class2=CHipsDlg
Class3=CAboutDlg

ResourceCount=5
Resource1=IDD_DLG_TIP
Resource2=IDR_MAINFRAME
Resource3=IDD_ABOUTBOX
Class4=CTips
Resource4=IDD_HIPS_DIALOG
Resource5=IDR_TRAYMENU

[CLS:CHipsApp]
Type=0
HeaderFile=Hips.h
ImplementationFile=Hips.cpp
Filter=N

[CLS:CHipsDlg]
Type=0
HeaderFile=HipsDlg.h
ImplementationFile=HipsDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_LIST_HOOKLOG

[CLS:CAboutDlg]
Type=0
HeaderFile=HipsDlg.h
ImplementationFile=HipsDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_HIPS_DIALOG]
Type=1
Class=CHipsDlg
ControlCount=5
Control1=IDC_BTN_ON,button,1342242816
Control2=IDC_BTN_OFF,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_LIST_HOOKLOG,SysListView32,1350631425
Control5=IDC_BTN_CLEARLOG,button,1342242816

[DLG:IDD_DLG_TIP]
Type=1
Class=CTips
ControlCount=4
Control1=IDC_EDIT_TEXT,edit,1352730628
Control2=IDC_BTN_OK,button,1342242816
Control3=IDC_BTN_CANCEL,button,1342242816
Control4=IDC_STATIC,static,1342308352

[CLS:CTips]
Type=0
HeaderFile=Tips.h
ImplementationFile=Tips.cpp
BaseClass=CDialog
Filter=D
LastObject=CTips
VirtualFilter=dWC

[MNU:IDR_TRAYMENU]
Type=1
Class=?
Command1=IDM_SHOW
Command2=IDM_EXIT
CommandCount=2

