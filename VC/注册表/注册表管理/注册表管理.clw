; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=SUBKEYCREATE
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "注册表管理.h"

ClassCount=5
Class1=CMyApp
Class2=CMyDlg

ResourceCount=5
Resource2=IDD_CREATE_VALUE
Resource3=IDD_CREATE_SUBKEY
Resource1=IDR_MAINFRAME
Class3=SUBKEYCREATE
Class4=SUBCREATE
Resource4=IDD_MY_DIALOG
Class5=VALUECREATE
Resource5=IDR_MENU_POP

[CLS:CMyApp]
Type=0
HeaderFile=注册表管理.h
ImplementationFile=注册表管理.cpp
Filter=N

[CLS:CMyDlg]
Type=0
HeaderFile=注册表管理Dlg.h
ImplementationFile=注册表管理Dlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_TREE



[DLG:IDD_MY_DIALOG]
Type=1
Class=CMyDlg
ControlCount=4
Control1=IDC_TREE,SysTreeView32,1350631427
Control2=IDC_LIST,SysListView32,1350631425
Control3=IDC_STATIC,static,1342312448
Control4=IDC_STATIC,static,1342312448

[MNU:IDR_MENU_POP]
Type=1
Class=SUBKEYCREATE
Command1=ID_MENU_DELETE_SUB
Command2=ID_MENU_DELETE_PRO
Command3=ID_MENU_CREATE_SUBKEY
Command4=ID_MENU_ADD_CHAR
Command5=ID_MENU_ADD_DWORD
Command6=ID_MENU_ADD_BINARY
Command7=ID_MENU_ABOUT
CommandCount=7

[CLS:SUBKEYCREATE]
Type=0
HeaderFile=SUBKEYCREATE.h
ImplementationFile=SUBKEYCREATE.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=ID_MENU_ADD_CHAR

[DLG:IDD_CREATE_SUBKEY]
Type=1
Class=SUBCREATE
ControlCount=4
Control1=IDC_EDIT1,edit,1350631552
Control2=IDOK,button,1342275585
Control3=IDCANCEL,button,1342275584
Control4=IDC_STATIC,static,1342308352

[CLS:SUBCREATE]
Type=0
HeaderFile=SUBCREATE.h
ImplementationFile=SUBCREATE.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_EDIT1

[DLG:IDD_CREATE_VALUE]
Type=1
Class=VALUECREATE
ControlCount=8
Control1=IDC_EDIT_NAME,edit,1350631552
Control2=IDC_EDIT_TYPE,edit,1350631552
Control3=IDC_EDIT_DATE,edit,1350631552
Control4=IDOK,button,1342275585
Control5=IDCANCEL,button,1342275584
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352

[CLS:VALUECREATE]
Type=0
HeaderFile=VALUECREATE.h
ImplementationFile=VALUECREATE.cpp
BaseClass=CDialog
Filter=D
LastObject=ID_MENU_ABOUT
VirtualFilter=dWC

