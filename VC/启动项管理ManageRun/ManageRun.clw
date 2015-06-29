; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CRegAdd
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ManageRun.h"

ClassCount=4
Class1=CManageRunApp
Class2=CManageRunDlg
Class3=CAboutDlg

ResourceCount=4
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_MANAGERUN_DIALOG
Class4=CRegAdd
Resource4=IDD_DLG_ADD

[CLS:CManageRunApp]
Type=0
HeaderFile=ManageRun.h
ImplementationFile=ManageRun.cpp
Filter=N

[CLS:CManageRunDlg]
Type=0
HeaderFile=ManageRunDlg.h
ImplementationFile=ManageRunDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CManageRunDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=ManageRunDlg.h
ImplementationFile=ManageRunDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_MANAGERUN_DIALOG]
Type=1
Class=CManageRunDlg
ControlCount=4
Control1=IDC_LIST_RUN,SysListView32,1350631429
Control2=IDC_BTN_ADD,button,1342242816
Control3=IDC_BTN_DEL,button,1342242816
Control4=IDC_BTN_QUIT,button,1342242816

[DLG:IDD_DLG_ADD]
Type=1
Class=CRegAdd
ControlCount=6
Control1=IDC_STATIC,static,1342308352
Control2=IDC_EDIT_KEYNAME,edit,1350631552
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT_KEYVALUE,edit,1350631552
Control5=IDC_BTN_OK,button,1342242816
Control6=IDC_BTN_CANCEL,button,1342242816

[CLS:CRegAdd]
Type=0
HeaderFile=RegAdd.h
ImplementationFile=RegAdd.cpp
BaseClass=CDialog
Filter=D
LastObject=CRegAdd
VirtualFilter=dWC

