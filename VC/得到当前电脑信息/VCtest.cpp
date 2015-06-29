// VCtest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "VCtest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object
CWinApp theApp;
using namespace std;

// 获取CPU相关信息
void GetCPUInfo()
{
	CString CPU;
	SYSTEM_INFO systeminfo;
	// 获取CPU型号
	GetSystemInfo (&systeminfo);
	switch (systeminfo.dwProcessorType)
	{
	case PROCESSOR_INTEL_386: // 386系列
		CPU = "PROCESSOR_INTEL_386";
		break;
	case PROCESSOR_INTEL_486: // 486系列
		CPU = "PROCESSOR_INTEL_486";
		break;
	case PROCESSOR_INTEL_PENTIUM: // 奔腾系列
		CPU = "PROCESSOR_INTEL_PENTIUM";
		break;
	case PROCESSOR_MIPS_R4000:
		CPU = "PROCESSOR_MIPS_R4000";
		break;
	case PROCESSOR_HITACHI_SH3:
		CPU = "PROCESSOR_HITACHI_SH3";
		break;
	case PROCESSOR_HITACHI_SH4:
		CPU = "PROCESSOR_HITACHI_SH4";
		break;
	case PROCESSOR_STRONGARM:
		CPU  = "PROCESSOR_STRONGARM";
		break;
	case PROCESSOR_ARM720:
		CPU = "PROCESSOR_ARM720";
		break;
	default:
		CPU = "Unknown Type";
		break;
	}
	printf("\n\nCPU 型号：%s\n\n",CPU);
	
}
// 获取内存信息
void GetMemoryInfo()
{
	CString MemoryTotal; // 总内存大小
	CString MemoryAvail; // 可用内存大小
	
	
	MEMORYSTATUS memory;
	memory.dwLength =sizeof(memory); //初始化
	// 获取内存信息
	GlobalMemoryStatus(&memory);
	MemoryTotal.Format(_T("%d Mb"),int(memory.dwTotalPhys/1024/1024));
	MemoryAvail.Format(_T("%d Mb"),int(memory.dwAvailPhys/1024/1024));
	printf("主机总内存：%s\n\n",MemoryTotal);
	printf("主机可用内存：%s\n\n",MemoryAvail);
}

// 获取磁盘信息
void GetDiskInfo()
{
	DWORD sector,byte,cluster,free;
	long int freespace,totalspace;
	UINT type;
	char name;
	CString strname;
	// 循环检测A～Z, 找到磁盘即输出
	//0—未知盘、1—不存在、2—可移动磁盘、3—固定磁盘
    //4—网络磁盘、5—CD－ROM、6—内存虚拟盘
	for (name='A';name<='Z';name++) 
	{
		strname.Format(_T("%c:"),name);
		type = GetDriveTypeA(strname); //获得磁盘类型
		switch(type)
		{
		case 0:
			printf("未知类型磁盘: %s",strname);
			break;
		case 2:
			printf("可移动类型磁盘: %s",strname);
			break;
		case 3:
			printf("固定磁盘: %s",strname);
			break;
		case 4:
			printf("网络映射磁盘: %s",strname);
			break;
		case 5:
			printf("光驱: %s",strname);
			break;
		case 6:
			printf("内存虚拟磁盘: %s",strname);
			break;
		default:
			break;
		}
		if(type != 1)
		{
			// 获取磁盘空间信息
			GetDiskFreeSpaceA(strname,&sector,&byte,&free,&cluster);
			// 磁盘总大小
			totalspace = int(cluster)*byte*sector/1024/1024;
			// 磁盘可用空间大小
			freespace = int(free)*byte*sector/1024/1024; 
			printf(" 总大小 %d Mb",totalspace);
			printf(" 可用空间 %d Mb",freespace);
			printf("\n");
		}
	}

}
int  main(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		// 输出CPU信息
		GetCPUInfo();
		// 输出内存信息
		GetMemoryInfo();
		// 输出磁盘信息
		GetDiskInfo();
	}

	return nRetCode;
}
