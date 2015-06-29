; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CUFirewallDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "UFirewall.h"

ClassCount=3
Class1=CUFirewallApp
Class2=CUFirewallDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_UFIREWALL_DIALOG

[CLS:CUFirewallApp]
Type=0
HeaderFile=UFirewall.h
ImplementationFile=UFirewall.cpp
Filter=N

[CLS:CUFirewallDlg]
Type=0
HeaderFile=UFirewallDlg.h
ImplementationFile=UFirewallDlg.cpp
Filter=D
LastObject=IDC_BTN_SAFEOPEN
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=UFirewallDlg.h
ImplementationFile=UFirewallDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_UFIREWALL_DIALOG]
Type=1
Class=CUFirewallDlg
ControlCount=1
Control1=IDC_BTN_SAFEOPEN,button,1342242816

