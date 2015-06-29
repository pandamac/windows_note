; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAboutDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "服务管理.h"

ClassCount=3
Class1=CMyApp
Class2=CMyDlg
Class3=CAboutDlg

ResourceCount=4
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_MY_DIALOG
Resource4=IDR_MENU_POPUP

[CLS:CMyApp]
Type=0
HeaderFile=服务管理.h
ImplementationFile=服务管理.cpp
Filter=N
LastObject=CMyApp

[CLS:CMyDlg]
Type=0
HeaderFile=服务管理Dlg.h
ImplementationFile=服务管理Dlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=ID_MENU_START_MANAGER

[CLS:CAboutDlg]
Type=0
HeaderFile=服务管理Dlg.h
ImplementationFile=服务管理Dlg.cpp
Filter=D
LastObject=ID_MENU_START_AUTO
BaseClass=CDialog
VirtualFilter=dWC

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=13
Control1=IDC_ADD_SERVER_NAME,edit,1350631552
Control2=IDC_ADD_SERVER_INFO,edit,1350631552
Control3=IDC_ADD_SERVER_PATH,edit,1350631552
Control4=IDC_BUTTON_OPEN_FILE,button,1342242816
Control5=IDOK,button,1342373889
Control6=IDOK2,button,1342373889
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,button,1342308359
Control12=IDC_RADIO1,button,1342177289
Control13=IDC_RADIO2,button,1342177289

[DLG:IDD_MY_DIALOG]
Type=1
Class=CMyDlg
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_LIST1,SysListView32,1350631425
Control4=IDC_STATIC,static,1342308352

[MNU:IDR_MENU_POPUP]
Type=1
Class=CAboutDlg
Command1=ID_MENU_REF
Command2=ID_MENU_START
Command3=ID_MENU_STOP
Command4=ID_MENU_DEL
Command5=ID_MENU_CREATE
Command6=ID_MENU_START_AUTO
Command7=ID_MENU_START_MANAGER
Command8=ID_MENU_START_DISABLE
Command9=ID_MENU_ABOUT
CommandCount=9

