; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CCreate
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ManageProcess.h"

ClassCount=4
Class1=CManageProcessApp
Class2=CManageProcessDlg
Class3=CAboutDlg

ResourceCount=4
Resource1=IDD_MANAGEPROCESS_DIALOG
Resource2=IDR_MAINFRAME
Resource3=IDD_ABOUTBOX
Class4=CCreate
Resource4=IDD_DLG_CREATE

[CLS:CManageProcessApp]
Type=0
HeaderFile=ManageProcess.h
ImplementationFile=ManageProcess.cpp
Filter=N

[CLS:CManageProcessDlg]
Type=0
HeaderFile=ManageProcessDlg.h
ImplementationFile=ManageProcessDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CManageProcessDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=ManageProcessDlg.h
ImplementationFile=ManageProcessDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_MANAGEPROCESS_DIALOG]
Type=1
Class=CManageProcessDlg
ControlCount=8
Control1=IDC_LIST_PROCESS,SysListView32,1350631425
Control2=IDC_BTN_CREATE,button,1342242816
Control3=IDC_BTN_TERMINATE,button,1342242816
Control4=IDC_BTN_STOP,button,1342242816
Control5=IDC_BTN_RESUME,button,1342242816
Control6=IDC_BTN_EXIT,button,1342242816
Control7=IDC_BTN_SHOWMODULE,button,1342242816
Control8=IDC_LIST_MODULE,SysListView32,1350631425

[DLG:IDD_DLG_CREATE]
Type=1
Class=CCreate
ControlCount=4
Control1=IDC_STATIC,static,1342308352
Control2=IDC_EDIT_FILEPATH,edit,1350631552
Control3=IDC_BTN_OK,button,1342242816
Control4=IDC_BTN_CANCEL,button,1342242816

[CLS:CCreate]
Type=0
HeaderFile=Create.h
ImplementationFile=Create.cpp
BaseClass=CDialog
Filter=D
LastObject=CCreate
VirtualFilter=dWC

