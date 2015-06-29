// 注册表管理Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "注册表管理.h"
#include "注册表管理Dlg.h"
#include "SUBCREATE.h"
#include "VALUECREATE.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/**************************************************************/

CImageList	m_HeadIcon;
HKEY		hKey;			//注册表返回句柄
HKEY		MKEY;			//用于保存主键
CString		FullPath = "";
HTREEITEM	SelectNode=NULL;

/**************************************************************/



/////////////////////////////////////////////////////////////////////////////
// CMyDlg dialog

CMyDlg::CMyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyDlg)
	DDX_Control(pDX, IDC_TREE, m_tree);
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyDlg, CDialog)
	//{{AFX_MSG_MAP(CMyDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE, OnSelchangingTree)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_MENU_DELETE_SUB, OnMenuDeleteSub)
	ON_COMMAND(ID_MENU_DELETE_PRO, OnMenuDeletePro)
	ON_COMMAND(ID_MENU_CREATE_SUBKEY, OnMenuCreateSubkey)
	ON_COMMAND(ID_MENU_ADD_BINARY, OnMenuAddBinary)
	ON_COMMAND(ID_MENU_ADD_CHAR, OnMenuAddChar)
	ON_COMMAND(ID_MENU_ADD_DWORD, OnMenuAddDword)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyDlg message handlers

BOOL CMyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();



	m_list.InsertColumn(0,"名称",LVCFMT_LEFT,120,-1);
	m_list.InsertColumn(1,"类型",LVCFMT_LEFT,60,-1);
	m_list.InsertColumn(2,"数据",LVCFMT_LEFT,210,-1);

	//////添加图标//////
	m_HeadIcon.Create(16,16,TRUE,2,2);
	m_HeadIcon.Add(AfxGetApp()->LoadIcon(IDI_ICON3));
	m_HeadIcon.Add(AfxGetApp()->LoadIcon(IDI_ICON4));

	m_list.SetImageList(&m_HeadIcon,LVSIL_SMALL);


	//树控件设置

	HICON hIcon = NULL;
	m_ImageList_tree.Create(18, 18, ILC_COLOR16,10, 0);

	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 18, 18, 0);
	m_ImageList_tree.Add(hIcon);
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 32, 32, 0);
	m_ImageList_tree.Add(hIcon);



	m_tree.SetImageList ( &m_ImageList_tree,TVSIL_NORMAL );

	DWORD	dwStyle = GetWindowLong(m_tree.m_hWnd,GWL_STYLE);

    dwStyle |=TVS_HASBUTTONS | TVS_HASLINES;// | TVS_LINESATROOT;这里不可一设置这个样式不然ico图表无法显示

    SetWindowLong(m_tree.m_hWnd,GWL_STYLE,dwStyle);
    m_hRoot = m_tree.InsertItem("注册表管理",0,0,0,0);   

	HKCU=m_tree.InsertItem("HKEY_CURRENT_USER",1,1,m_hRoot,0);
	HKLM=m_tree.InsertItem("HKEY_LOCAL_MACHINE",1,1,m_hRoot,0);
	HKUS=m_tree.InsertItem("HKEY_USERS",1,1,m_hRoot,0);
	HKCC=m_tree.InsertItem("HKEY_CURRENT_CONFIG",1,1,m_hRoot,0);
	HKCR=m_tree.InsertItem("HKEY_CLASSES_ROOT",1,1,m_hRoot,0);

	m_tree.Expand(m_hRoot,TVE_EXPAND);


	//SetIcon(m_hIcon, TRUE);			// Set big icon
	//SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMyDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CMyDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMyDlg::OnSelchangingTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	TVITEM item = pNMTreeView->itemNew;

	m_list.DeleteAllItems();

	SelectNode=item.hItem;			//保存用户打开的子树节点句柄

	if(item.hItem == m_hRoot)
	{
		return;
	}

	
    HTREEITEM CurrentNode =  item.hItem; //取得此节点的全路径

	while(m_tree.GetChildItem(CurrentNode)!=NULL)
	{
		m_tree.DeleteItem(m_tree.GetChildItem(CurrentNode));
	}


	FullPath = "";
    CString Temp;
	while(CurrentNode != m_hRoot)
	{
		Temp = m_tree.GetItemText(CurrentNode);
		if(Temp.Right(1) != "\\")
		{
			Temp += "\\";
		}
		FullPath = Temp + FullPath;
		CurrentNode = m_tree.GetParentItem(CurrentNode);
	}

	GetDlgItem(IDC_STATIC)->SetWindowText(FullPath.LockBuffer());	//设置状态栏



	if(!FullPath.Find("HKEY_CLASSES_ROOT"))	//判断主键
	{
		MKEY=HKEY_CLASSES_ROOT;
		FullPath.Delete(0,sizeof("HKEY_CLASSES_ROOT"));
	}else
	if(!FullPath.Find("HKEY_CURRENT_USER"))
	{
		MKEY=HKEY_CURRENT_USER;
		FullPath.Delete(0,sizeof("HKEY_CURRENT_USER"));

	}else
	if(!FullPath.Find("HKEY_LOCAL_MACHINE"))
	{
		MKEY=HKEY_LOCAL_MACHINE;
		FullPath.Delete(0,sizeof("HKEY_LOCAL_MACHINE"));

	}else
	if(!FullPath.Find("HKEY_USERS"))
	{
		MKEY=HKEY_USERS;
		FullPath.Delete(0,sizeof("HKEY_USERS"));

	}else
	if(!FullPath.Find("HKEY_CURRENT_CONFIG"))
	{
		MKEY=HKEY_CURRENT_CONFIG;
		FullPath.Delete(0,sizeof("HKEY_CURRENT_CONFIG"));

	}


	if(RegOpenKeyEx(MKEY,FullPath.LockBuffer(),0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)//打开
	{

		char	*szValueName;		//键值名称
		char	*szKeyName;			//子键名称
		LPBYTE	szValueDate;		//键值数据

		DWORD dwIndex=0,NameSize,NameCnt,NameMaxLen,Type;
		DWORD KeySize,KeyCnt,KeyMaxLen,DateSize,MaxDateLen;

		if(RegQueryInfoKey(hKey,NULL,NULL,NULL,&KeyCnt,&KeyMaxLen,NULL,&NameCnt,&NameMaxLen,&MaxDateLen,NULL,NULL)!=ERROR_SUCCESS)
		{
			AfxMessageBox("RegQueryInfoKey错误");
			return;
		}

		int nitem=m_list.InsertItem(0,"(默认)",0);
		m_list.SetItemText(nitem,1,"REG_SZ");	
		m_list.SetItemText(nitem,2,"(数据未设置)");


		for(dwIndex=0;dwIndex<NameCnt;dwIndex++)	//枚举键值
		{
			DateSize=MaxDateLen+1;
			NameSize=NameMaxLen+100;
			szValueName=(char *)malloc(NameSize);
			szValueDate=(LPBYTE)malloc(DateSize);
			RegEnumValue(hKey,dwIndex,szValueName,&NameSize,NULL,&Type,szValueDate,&DateSize);//读取键值


			if(Type==REG_SZ)
			{
				int nitem=m_list.InsertItem(0,szValueName,0);
				m_list.SetItemText(nitem,1,"REG_SZ");	
				m_list.SetItemText(nitem,2,(char*)szValueDate);
			}
			if(Type==REG_DWORD)
			{
				char ValueDate[256];
				sprintf(ValueDate,"%wd",szValueDate);

				int nitem=m_list.InsertItem(0,szValueName,1);
				m_list.SetItemText(nitem,1,"REG_DWORD");	
				m_list.SetItemText(nitem,2,ValueDate);
			}
			if(Type==REG_BINARY)
			{
				char ValueDate[256];
				sprintf(ValueDate,"%wd",szValueDate);

				int nitem=m_list.InsertItem(0,szValueName,1);
				m_list.SetItemText(nitem,1,"REG_BINARY");	
				m_list.SetItemText(nitem,2,ValueDate);
			}
			if(Type==REG_EXPAND_SZ)
			{
				int nitem=m_list.InsertItem(0,szValueName,0);
				m_list.SetItemText(nitem,1,"REG_EXPAND_SZ");	
				m_list.SetItemText(nitem,2,(char*)szValueDate);
			}

		}


		for(dwIndex=0;dwIndex<KeyCnt;dwIndex++)		//枚举子键
		{
			KeySize=KeyMaxLen+1;
			szKeyName=(char*)malloc(KeySize);
			RegEnumKeyEx(hKey,dwIndex,szKeyName,&KeySize,NULL,NULL,NULL,NULL);

			m_tree.InsertItem(szKeyName,1,1,SelectNode,0);//插入子键名称
			m_tree.Expand(SelectNode,TVE_EXPAND);

		}
		RegCloseKey(hKey);
	}
	else
	{
		AfxMessageBox("打开注册表失败 或者 权限不够");
	}

	
	*pResult = 0;
}

void CMyDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu menu;
	menu.LoadMenu(IDR_MENU_POP);
	CMenu *pop = menu.GetSubMenu(0);

	POINT ptMouse;
	GetCursorPos(&ptMouse);


	pop->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,ptMouse.x,ptMouse.y,this);
		
}

void CMyDlg::OnMenuDeleteSub() 
{

	CString FileBuffer;
	FileBuffer=CString(TEXT("确实要删除")+FullPath+TEXT("子键吗?"));

	if(AfxMessageBox(FileBuffer,MB_OKCANCEL,0)==IDOK)
	{
		if(ERROR_SUCCESS==RegDeleteKey(MKEY,FullPath.LockBuffer()))
		{
			AfxMessageBox("删除成功!");
		}else
		{
			AfxMessageBox("删除失败!");
		}
	}
}

void CMyDlg::OnMenuDeletePro() 
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	int currentSelect=m_list.GetNextSelectedItem(pos);

	if(currentSelect>=0)
	{	
		CString FileBuffer;		//构建确认对话框
		FileBuffer=CString(TEXT("确实要删除")+m_list.GetItemText(currentSelect,0)+TEXT("键值项吗?"));

		if(AfxMessageBox(FileBuffer,MB_OKCANCEL,0)==IDOK)
		{
			HKEY key;

			CString value=m_list.GetItemText(currentSelect,0);			//键值
			LPCTSTR data=FullPath.LockBuffer();							//路径

			::RegOpenKeyEx(MKEY,data,0,KEY_WRITE,&key);					//打开

			if(ERROR_SUCCESS==::RegDeleteValue(key,value.LockBuffer()))	//删除
			{
				AfxMessageBox("删除成功!");
			}else
			{
				AfxMessageBox("删除失败!");
			}
		}
	}else
	{
			AfxMessageBox("请选择需要删除的键值项");
	}
}



void CMyDlg::OnMenuCreateSubkey() 
{
	SUBCREATE dlg;
	dlg.SetDate(MKEY,FullPath);
	dlg.DoModal();
}

void CMyDlg::OnMenuAddBinary() 
{
	VALUECREATE dlg;
	dlg.SetDate(MKEY,FullPath);
	dlg.DoModal();
}

void CMyDlg::OnMenuAddChar() 
{
	VALUECREATE dlg;
	dlg.SetDate(MKEY,FullPath);
	dlg.DoModal();	
}

void CMyDlg::OnMenuAddDword() 
{
	VALUECREATE dlg;
	dlg.SetDate(MKEY,FullPath);
	dlg.DoModal();	
}
