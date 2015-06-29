/*
 *                                     SSDT操作函数
 *                                     Made By Adly
 *
 *                                     QQ:369046978
 *
 */
/////////////////////////////////
//SSDT.cpp
#include "SSDT.h"

ULONG NTBase = 0;			//NT 基址
ULONG TotalSSDTCount = -1;		//SSDT服务个数
pSSDTSaveTable pSSDTST = NULL;	//保存的SSDT缓冲表
PSYSMODULELIST pList = NULL;	//模块信息链表
ULONG RealCount = 0;		//枚举所找到的服务个数

pNtQuerySystemInformationProto pNtQuerySystemInformation = NULL;
///////////////////////////////////////////////////
//加载驱动
HANDLE LoadDriver( IN LPCTSTR lpFileName )
{
    HANDLE hDriver = INVALID_HANDLE_VALUE;
	char OpenName[MAX_PATH+1];
	sprintf( OpenName, "\\\\.\\%s", EXE_DRIVER_NAME );
    SC_HANDLE hSCManager = OpenSCManager( NULL, NULL,
        SC_MANAGER_CREATE_SERVICE );
    if ( NULL != hSCManager )
    {
        SC_HANDLE hService = CreateService( hSCManager, EXE_DRIVER_NAME,
            DISPLAY_NAME, SERVICE_START,
            SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START,
            SERVICE_ERROR_IGNORE, lpFileName, NULL, NULL, NULL, NULL, NULL );
        if ( ERROR_SERVICE_EXISTS == GetLastError() )
        {
            hService = OpenService( hSCManager, EXE_DRIVER_NAME, SERVICE_START );
        }
        if( !StartService( hService, 0, NULL ) )
		{
			if( GetLastError() != 1056 )	//已经启动
			{
				//启动失败
				// Do Nothing
			}
			else
			{
				//服务已经启动
				// Do Nothing
			}
		}
        CloseServiceHandle( hService );
        CloseServiceHandle( hSCManager );
        hDriver = CreateFileA( OpenName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );
		if( hDriver == INVALID_HANDLE_VALUE )
		{
			//获取文件句柄失败
			// Do Nothing
		}
    }
    return hDriver;
}
//卸载驱动
void UnloadDriver( IN HANDLE hDriver )
{
    CloseHandle( hDriver );

    SC_HANDLE hSCManager = OpenSCManager( NULL, NULL,
        SC_MANAGER_CREATE_SERVICE );
    if ( NULL != hSCManager )
    {
        SC_HANDLE hService = OpenService( hSCManager, EXE_DRIVER_NAME, DELETE | SERVICE_STOP );
        if ( NULL != hService )
        {
            SERVICE_STATUS ss;
            ControlService( hService, SERVICE_CONTROL_STOP, &ss );
            DeleteService( hService );
            CloseServiceHandle( hService );
        }
        CloseServiceHandle( hSCManager );
    }
}
//得到SSDT
BOOL GetSSDT( IN HANDLE hDriver, OUT PSSDT ssdt )
{
	if( ssdt == NULL )
	{
		return FALSE;
	}
	DWORD dwRet;
	BOOL bRet = DeviceIoControl( hDriver, IOCTL_GETSSDT, NULL, 0, ssdt, sizeof( SSDT ), \
		&dwRet, NULL );
	if( bRet )
	{
		TotalSSDTCount = ssdt->ulNumberOfServices;		//获取服务个数
		return TRUE;
	}
	return FALSE;
}
//设置SSDT
BOOL SetSSDT( IN HANDLE hDriver, IN PSSDT ssdt )
{
	if( ssdt == NULL )
	{
		return FALSE;
	}
	DWORD dwRet;
	BOOL bRet = DeviceIoControl( hDriver, IOCTL_SETSSDT, ssdt, sizeof( SSDT ), \
		NULL, 0, &dwRet, NULL );
	if( bRet )
	{
		TotalSSDTCount = ssdt->ulNumberOfServices;		//获取服务个数
		return TRUE;
	}
	return FALSE;
}
//得到SSDT表中HOOK的地址
BOOL GetHook( IN HANDLE hDriver, IN ULONG ulIndex, OUT PULONG ulAddr )
{
	if( ulAddr == NULL )
	{
		return FALSE;
	}
	DWORD dwRet;
	BOOL bRet = DeviceIoControl( hDriver, IOCTL_GETHOOK, &ulIndex, sizeof( ULONG ), \
		ulAddr, sizeof( ULONG ), &dwRet, NULL );

	return bRet;
}
//设置SSDT表中HOOK的地址
BOOL SetHook( IN HANDLE hDriver, IN ULONG ulIndex, IN OUT PULONG ulAddr )
{
	if( ulAddr == NULL )
	{
		return FALSE;
	}
	DWORD dwRet;
	BOOL bRet = DeviceIoControl( hDriver, IOCTL_SETHOOK, &ulIndex, sizeof( ULONG ), \
		ulAddr, sizeof( ULONG ), &dwRet, NULL );
	return bRet;
}
//创建模块列表
PSYSMODULELIST CreateModList( OUT PULONG ulNtBase )
{
	HINSTANCE hNTDll;
	ULONG nRet;
	ULONG nQuerySize;
	ULONG Success;
	PSYSMODULELIST pModInfo = NULL;

	do
	{
		if( !( hNTDll = LoadLibrary( "ntdll" ) ) )
		{
			break;
		}
		if( pNtQuerySystemInformation == NULL )
		{
			pNtQuerySystemInformation = (pNtQuerySystemInformationProto)GetProcAddress( hNTDll, "NtQuerySystemInformation" );
			if( !pNtQuerySystemInformation )
			{
				break;
			}
		}
		Success = pNtQuerySystemInformation( \
			SystemModuleInfo, 
			NULL, 
			0, 
			&nQuerySize
			);
		pModInfo = (PSYSMODULELIST)malloc( nQuerySize );
		if( !pModInfo )
		{
			break;
		}
		Success = pNtQuerySystemInformation( \
			SystemModuleInfo, 
			pModInfo, 
			nQuerySize, 
			&nRet
			);
		if( Success < 0 )
		{
			free( pModInfo );
			pModInfo = NULL;
			break;
		}
		
		*ulNtBase = (ULONG)(pModInfo->smi->Base);	//顺便得到NT基址(ntoskrnl.exe在内核中的加载基址) ^_^
		
	} while( FALSE );

	::FreeLibrary( hNTDll );

	return pModInfo;
}
//消毁列表
void DestroyModList( IN PSYSMODULELIST pList )
{
	free( pList );
}
//通过地址得到模块名
BOOL GetModuleNameByAddr( IN ULONG ulAddr, IN PSYSMODULELIST pList, OUT LPSTR buf, IN DWORD dwSize )
{
	ULONG i;
	
	for ( i = 0; i < pList->ulCount; i++ )
	{
		ULONG ulBase = (ULONG)pList->smi[i].Base;
		ULONG ulMax  = ulBase + pList->smi[i].Size;
		if ( ulBase <= ulAddr && ulAddr < ulMax )
		{
			
			lstrcpynA( buf, pList->smi[i].ImageName, dwSize );
			
			return TRUE;
		}
	}
	return FALSE;
}
//根据服务号将SSDT缓冲表排序
BOOL SSDTSTOrderByServiceNum( pSSDTSaveTable pSsdtST )
{
	ULONG ulMaxServiceNumber = 0;		//服务号中的最大值
	ULONG i, j;
	//找出最大的
	for( i = 0; i < RealCount; i ++ )
	{
		ULONG ulCurServiceNum;
		ulCurServiceNum = ((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ulServiceNumber;
		ulMaxServiceNumber = ulCurServiceNum > ulMaxServiceNumber? ulCurServiceNum : ulMaxServiceNumber;
	}
	//根据最大服务号设置后面没有服务号的
	for( i = RealCount; i < TotalSSDTCount; i ++ )
	{
		ulMaxServiceNumber ++;		//最大号依次加,来设置后面的服务号
		((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ulServiceNumber = \
			ulMaxServiceNumber;
	}
	//按服务号排序
	//冒泡排序法
	for( i = TotalSSDTCount - 1; i > 0; i -- )
	{
		for( j = 0; j < i; j ++ )
		{
			if( \
			((pSSDTSaveTable)((ULONG)pSSDTST + j * sizeof(SSDTSaveTable)))->ulServiceNumber \
			> ((pSSDTSaveTable)((ULONG)pSSDTST + (j+1) * sizeof(SSDTSaveTable)))->ulServiceNumber \
			)	//交换
			{
				SSDTSaveTable SSDTSTTemp;
				memcpy( \
					(void*)&SSDTSTTemp, \
					(void*)((ULONG)pSSDTST + j * sizeof(SSDTSaveTable)), \
					sizeof( SSDTSaveTable ) \
					);
				memcpy( \
					(void*)((ULONG)pSSDTST + j * sizeof(SSDTSaveTable)), \
					(void*)((ULONG)pSSDTST + (j+1) * sizeof(SSDTSaveTable)), \
					sizeof( SSDTSaveTable ) \
					);
				memcpy( \
					(void*)((ULONG)pSSDTST + (j+1) * sizeof(SSDTSaveTable)), \
					(void*)&SSDTSTTemp, \
					sizeof( SSDTSaveTable ) \
					);
			}
		}
	}

	return TRUE;
}
///////////////////////////
//获取原始函数地址声明
void GetOldSSDTAddress();

//枚举SSDT
BOOL EnumSSDT( IN HANDLE hDriver )
{
	HINSTANCE hNtDllInst = NULL;
	ULONG ulNtDllOffset;
	ULONG ulFuncNameCount = 0;
	PIMAGE_EXPORT_DIRECTORY pImgExpDir = NULL;
	PULONG pFuncNameArray = NULL;
	ULONG i;
	BOOL bOK = TRUE;
	
	do
	{
		RealCount = 0;			//个数清0
		if( pList )		//还有存没有释放
		{
			DestroyModList( pList );	//释放它
			pList = NULL;
		}
		pList = CreateModList( &NTBase );	//创建模块信息链表,顺便得到NT基址
		if( pList == NULL )		//创建失败
		{
			bOK = FALSE;
			break;
		}
		
		if( !( hNtDllInst = LoadLibrary( "ntdll" ) ) )
		{
			bOK = FALSE;
			break;
		}
		/////////////////////////////////////////////////////////
		//分配SSDT保存缓冲表
		//得到SSDT个数
		SSDT ssdt;
		if( !GetSSDT( hDriver, &ssdt ) )
		{
			bOK = FALSE;
			break;
		}
		if( TotalSSDTCount == -1 )		//得到SSDT个数失败
		{
			bOK = FALSE;
			break;
		}
		if( pSSDTST )		//pSSDTST已有值,先释放它
		{
			free( pSSDTST );
			pSSDTST = NULL;
		}
		pSSDTST = (pSSDTSaveTable)malloc( TotalSSDTCount * sizeof( SSDTSaveTable ) );
		if( pSSDTST == NULL )
		{
			bOK = FALSE;
			break;
		}
		for( i = 0; i < TotalSSDTCount; i ++ )	//初始化它
		{
			((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ulServiceNumber = -1;
			((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ulCurrentFunctionAddress = 0L;
			((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ulOriginalFunctionAddress = 0L;
			memset( ((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ServiceFunctionName, \
				0, \
				sizeof(((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ServiceFunctionName));
			memset( ((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ModuleName, \
				0, \
				sizeof(((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ModuleName));
		}
		/////////////////////////////////////////////////////////
		//枚举
		ulNtDllOffset = (ULONG)hNtDllInst;
		//PE头部
		ulNtDllOffset += ((PIMAGE_DOS_HEADER)hNtDllInst)->e_lfanew + sizeof( DWORD );
		//数据目录
		ulNtDllOffset += sizeof( IMAGE_FILE_HEADER ) + sizeof( IMAGE_OPTIONAL_HEADER )
			- IMAGE_NUMBEROF_DIRECTORY_ENTRIES * sizeof( IMAGE_DATA_DIRECTORY );
		//导出表
		ulNtDllOffset = (DWORD)hNtDllInst + ((PIMAGE_DATA_DIRECTORY)ulNtDllOffset)->VirtualAddress;
		//导出目录表
		pImgExpDir = (PIMAGE_EXPORT_DIRECTORY)ulNtDllOffset;
		//得到函数个数
		ulFuncNameCount = pImgExpDir->NumberOfNames;
		//函数名数组指针
		pFuncNameArray = (PULONG)( (ULONG)hNtDllInst + pImgExpDir->AddressOfNames );
		/////////////////////
		//循环找函数名
		for( i = 0; i < ulFuncNameCount; i ++ )
		{
			//函数名
			PCSTR pszName = (PCSTR)( pFuncNameArray[i] + (ULONG)hNtDllInst );
			if( pszName[0] == 'N' && pszName[1] == 't' )	//Nt 开头的函数
			{
				//查找表
				LPWORD pOrdNameArray = (LPWORD)( (ULONG)hNtDllInst + pImgExpDir->AddressOfNameOrdinals );
				//函数地址
				LPDWORD pFuncArray = (LPDWORD)( (ULONG)hNtDllInst + pImgExpDir->AddressOfFunctions );
				//函数代码
				LPCVOID pFuncCode = (LPCVOID)( (ULONG)hNtDllInst + pFuncArray[pOrdNameArray[i]] );
				//获取服务号
				SSDTEntry EntryCode;
				memcpy( &EntryCode, pFuncCode, sizeof( SSDTEntry ) );
				if( EntryCode.byMov == 0xB8 )	// MOV EAX, XXXX
				{
					ULONG ulAddr = 0;
					if( !GetHook( hDriver, EntryCode.ulIndex, &ulAddr ) )
					{
						bOK = FALSE;
						break;
					}
					////////////////////////
					//通过地址得到模块名
					char ModNameBuf[MAX_PATH+1];
					memset( ModNameBuf, 0, sizeof( ModNameBuf ) );

					if( GetModuleNameByAddr( ulAddr, pList, ModNameBuf, sizeof( ModNameBuf )-1 ) )
					{
						memcpy( \
							((pSSDTSaveTable)((ULONG)pSSDTST + RealCount * sizeof(SSDTSaveTable)))->ModuleName, \
							ModNameBuf, \
							sizeof( ModNameBuf ) \
							);
					}
					////////////////////////////////////////////////////
					//保存SSDT信息到缓冲表中
					((pSSDTSaveTable)((ULONG)pSSDTST + RealCount * sizeof(SSDTSaveTable)))->ulServiceNumber = \
						EntryCode.ulIndex;	//服务号
					((pSSDTSaveTable)((ULONG)pSSDTST + RealCount * sizeof(SSDTSaveTable)))->ulCurrentFunctionAddress = \
						ulAddr;		//当前函数地址
					memcpy( \
						((pSSDTSaveTable)((ULONG)pSSDTST + RealCount * sizeof(SSDTSaveTable)))->ServiceFunctionName, \
						pszName, \
						sizeof( ((pSSDTSaveTable)((ULONG)pSSDTST + RealCount * sizeof(SSDTSaveTable)))->ServiceFunctionName )
						);
					
					RealCount ++;
				}
			}
		}
	} while( FALSE );

	::FreeLibrary( hNtDllInst );
	
	if( bOK )	//成功
	{
		//获取剩下的服务号
		for( i = RealCount; i < TotalSSDTCount; i ++ )
		{
			if( !GetHook( hDriver, i, &((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ulCurrentFunctionAddress ) )
			{
				bOK = FALSE;
				break;
			}
			////////////////////////
			//通过地址得到模块名
			char ModNameBuf[MAX_PATH+1];
			memset( ModNameBuf, 0, sizeof( ModNameBuf ) );
			
			if( GetModuleNameByAddr( \
				((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ulCurrentFunctionAddress, \
				pList, ModNameBuf, sizeof( ModNameBuf )-1 ) )
			{
				memcpy( \
					((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ModuleName, \
					ModNameBuf, \
					sizeof( ModNameBuf ) \
					);
			}
		}
		//按服务号进行排序
		SSDTSTOrderByServiceNum( pSSDTST );

		//获取原始函数地址
		GetOldSSDTAddress();
	}

	if( pList )
	{
		DestroyModList( pList );	//释放模块链表
		pList = NULL;
	}

	return bOK;
}
//恢复SSDT
BOOL ReSSDT( IN HANDLE hDriver )
{
	ULONG i;
	if( RealCount == 0 )
	{
		return FALSE;
	}
	for( i = 0; i < RealCount; i ++ )
	{
		if( \
			((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ulCurrentFunctionAddress \
			!= ((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ulOriginalFunctionAddress \
			)	//当前地址和原始地址不同,恢复原始地址值
		{
			if( !SetHook( hDriver, \
				((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ulServiceNumber, \
				&(((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ulOriginalFunctionAddress) \
				) )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}
//恢复SSDT并去掉非系统表
BOOL ReSSDTAndThrowSpilth( IN HANDLE hDriver )
{
	if( !ReSSDT( hDriver ) )
	{
		return FALSE;
	}
	//读取SSDT
	SSDT ssdt;
	if( !GetSSDT( hDriver, &ssdt ) )
	{
		return FALSE;
	}
	if( RealCount == 0 )
	{
		return FALSE;
	}
	ssdt.ulNumberOfServices = RealCount;
	if( !SetSSDT( hDriver, &ssdt ) )
	{
		return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////
//获取原始函数地址,以便恢复SSDT
#define ibaseDD *(PDWORD)&ibase
#define RVATOVA(base,offset) ((PVOID)((DWORD)(base)+(DWORD)(offset)))

typedef struct { 
	WORD    offset:12;
	WORD    type:4;
} IMAGE_FIXUP_ENTRY, *PIMAGE_FIXUP_ENTRY;


DWORD GetHeaders(PCHAR ibase,
				 PIMAGE_FILE_HEADER *pfh,
				 PIMAGE_OPTIONAL_HEADER *poh,
				 PIMAGE_SECTION_HEADER *psh)

{ 
	PIMAGE_DOS_HEADER mzhead=(PIMAGE_DOS_HEADER)ibase;

	if    ((mzhead->e_magic!=IMAGE_DOS_SIGNATURE) ||
		(ibaseDD[mzhead->e_lfanew]!=IMAGE_NT_SIGNATURE))
		return FALSE;

	*pfh=(PIMAGE_FILE_HEADER)&ibase[mzhead->e_lfanew];
	if (((PIMAGE_NT_HEADERS)*pfh)->Signature!=IMAGE_NT_SIGNATURE) 
		return FALSE;
	*pfh=(PIMAGE_FILE_HEADER)((PBYTE)*pfh + sizeof(IMAGE_NT_SIGNATURE));

	*poh=(PIMAGE_OPTIONAL_HEADER)((PBYTE)*pfh+sizeof(IMAGE_FILE_HEADER));
	if ((*poh)->Magic!=IMAGE_NT_OPTIONAL_HDR32_MAGIC)
		return FALSE;

	*psh=(PIMAGE_SECTION_HEADER)((PBYTE)*poh+sizeof(IMAGE_OPTIONAL_HEADER));
	return TRUE;
}

DWORD FindKiServiceTable(HMODULE hModule,DWORD dwKSDT)
{ 
    PIMAGE_FILE_HEADER    pfh;
    PIMAGE_OPTIONAL_HEADER    poh;
    PIMAGE_SECTION_HEADER    psh;
    PIMAGE_BASE_RELOCATION    pbr;
    PIMAGE_FIXUP_ENTRY    pfe;
    
    DWORD    dwFixups=0,i,dwPointerRva,dwPointsToRva,dwKiServiceTable;
    BOOL    bFirstChunk;
	
    GetHeaders((PCHAR)hModule,&pfh,&poh,&psh);

    if ((poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress) &&
        (!((pfh->Characteristics)&IMAGE_FILE_RELOCS_STRIPPED))) {//存重定位表
        
        pbr=(PIMAGE_BASE_RELOCATION)RVATOVA(poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress,hModule);
		
        bFirstChunk=TRUE;

        while (bFirstChunk || pbr->VirtualAddress) { 
            bFirstChunk=FALSE;
			
            pfe=(PIMAGE_FIXUP_ENTRY)((DWORD)pbr+sizeof(IMAGE_BASE_RELOCATION));
			//sizeof(IMAGE_BASE_RELOCATION) 不包含后面的	TypeOffset   8
            for (i=0;i<(pbr->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION))>>1;i++,pfe++) { 
                if (pfe->type==IMAGE_REL_BASED_HIGHLOW) { 
                    dwFixups++;
                    dwPointerRva = pbr->VirtualAddress+pfe->offset;//需要重定位数据的指针

                    dwPointsToRva=*(PDWORD)((DWORD)hModule+dwPointerRva)-(DWORD)poh->ImageBase;

                    if (dwPointsToRva==dwKSDT) { //需要重定位数据的RVA

                        if (*(PWORD)((DWORD)hModule+dwPointerRva-2)==0x05c7) {

                            dwKiServiceTable=*(PDWORD)((DWORD)hModule+dwPointerRva+4)-poh->ImageBase;
                            return dwKiServiceTable;
                        }
                    }
                    
                } 

            }
            *(PDWORD)&pbr+=pbr->SizeOfBlock;
        }
    }    
    
    return 0;
}

void GetOldSSDTAddress()
{     
    HMODULE    hKernel;
    PCHAR    pKernelName;
    PDWORD    pService;
    PIMAGE_FILE_HEADER    pfh;
    PIMAGE_OPTIONAL_HEADER    poh;
    PIMAGE_SECTION_HEADER    psh;
	DWORD    dwKernelBase,dwServices=0;
	DWORD    dwKSDT;
	DWORD    dwKiServiceTable;
	
	ULONG n;

    pNtQuerySystemInformation(SystemModuleInfo,&n,0,&n);
	PULONG p = (PULONG)malloc( n*sizeof(ULONG) );
    pNtQuerySystemInformation(SystemModuleInfo,p,n*sizeof(*p),0);
	PSYSTEM_MODULE_INFORMATION module=PSYSTEM_MODULE_INFORMATION(p+1);

    dwKernelBase=(DWORD)module->Base;

    pKernelName=module->ModuleNameOffset+module->ImageName;

    hKernel=LoadLibraryEx(pKernelName,0,DONT_RESOLVE_DLL_REFERENCES);
    if (!hKernel) {
        return;
    }

    if (!(dwKSDT=(DWORD)GetProcAddress(hKernel,"KeServiceDescriptorTable"))) {
        return;
    }

    dwKSDT-=(DWORD)hKernel;

    if (!(dwKiServiceTable=FindKiServiceTable(hKernel,dwKSDT))) { 
        return;
    }

    GetHeaders((PCHAR)hKernel,&pfh,&poh,&psh);
    dwServices=0;

    for (pService=(PDWORD)((DWORD)hKernel+dwKiServiceTable);
	*pService-poh->ImageBase<poh->SizeOfImage;
	pService++,dwServices++)
	{
		((pSSDTSaveTable)((ULONG)pSSDTST + dwServices * sizeof(SSDTSaveTable)))->ulOriginalFunctionAddress = \
			*pService-poh->ImageBase+dwKernelBase;
	}
	FreeLibrary(hKernel);

	free( p );
}