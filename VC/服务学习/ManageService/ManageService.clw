; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CManageServiceDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ManageService.h"

ClassCount=3
Class1=CManageServiceApp
Class2=CManageServiceDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_MANAGESERVICE_DIALOG

[CLS:CManageServiceApp]
Type=0
HeaderFile=ManageService.h
ImplementationFile=ManageService.cpp
Filter=N

[CLS:CManageServiceDlg]
Type=0
HeaderFile=ManageServiceDlg.h
ImplementationFile=ManageServiceDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_LIST_SERVICE

[CLS:CAboutDlg]
Type=0
HeaderFile=ManageServiceDlg.h
ImplementationFile=ManageServiceDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_MANAGESERVICE_DIALOG]
Type=1
Class=CManageServiceDlg
ControlCount=6
Control1=IDC_LIST_SERVICE,SysListView32,1350631429
Control2=IDC_RADIO_WIN32,button,1342177289
Control3=IDC_RADIO_DRIVER,button,1342177289
Control4=IDC_BTN_START,button,1342242816
Control5=IDC_BTN_STOP,button,1342242816
Control6=IDC_BTN_EXIT,button,1342242816

