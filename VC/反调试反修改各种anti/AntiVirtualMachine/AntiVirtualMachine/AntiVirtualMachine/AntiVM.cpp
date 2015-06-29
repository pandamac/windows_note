#include "AntiVM.h"
#include <windows.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <Tlhelp32.h>

#pragma comment(lib,"Iphlpapi.lib")

AntiVM::AntiVM(void)
{
}

AntiVM::~AntiVM(void)
{
}



/*通过执行特权指令来检测虚拟机
Vmware为真主机与虚拟机之间提供了相互沟通的通讯机制，它使用“IN”指令来读取特定端口的数据以进行两机通讯，
但由于IN指令属于特权指令，在处于保护模式下的真机上执行此指令时，除非权限允许，
否则将会触发类型为“EXCEPTION_PRIV_INSTRUCTION”的异常，而在虚拟机中并不会发生异常，
在指定功能号0A（获取VMware版本）的情况下，它会在EBX中返回其版本号“VMXH”；而当功能号为0x14时，
可用于获取VMware内存大小，当大于0时则说明处于虚拟机中。VMDetect正是利用前一种方法来检测VMware的存在，其检测代码分析如下：*/


BOOL AntiVM::IsInsideVMWare_By_In()
{
	bool rc = true;

	__try
	{
		__asm
		{
				push   edx
				push   ecx
				push   ebx

				mov    eax, 'VMXh'
				mov    ebx, 0  // 将ebx设置为非幻数’VMXH’的其它值
				mov    ecx, 10 // 指定功能号，用于获取VMWare版本，当它为0x14时用于获取VMware内存大小
				mov    edx, 'VX' // 端口号
				in     eax, dx // 从端口dx读取VMware版本到eax
				//若上面指定功能号为0x14时，可通过判断eax中的值是否大于0，若是则说明处于虚拟机中
				cmp    ebx, 'VMXh' // 判断ebx中是否包含VMware版本’VMXh’，若是则在虚拟机中
				setz   [rc] // 设置返回值 rc = ZF

			pop    ebx
				pop    ecx
				pop    edx
		}
	}
	__except(1)  //如果未处于VMware中，则触发此异常 EXCEPTION_EXECUTE_HANDLER = 1
	{
		rc = false;
	}

	return rc;
}

// 原理：RDTSC指令是用于将计算机启动以来的CPU运行周期数存放到EDX：EAX里面，其中EDX是高位，而EAX是低位
// 在虚拟机中远比主机中慢
// 这里我们只需用低位计算
// 也可以anti debug 原理类似GetTickCount
BOOL AntiVM::IsInsideVmVare_By_RDTSC()
{
	BOOL bRet = FALSE;
	int value = 0;// xchg ecx, eax指令运行时间
	__asm
	{
		RDTSC
			xchg ecx, eax // 将运行周期数的地位给ecx
			RDTSC
			sub eax, ecx // 两次之差  高位-低位
			cmp eax, 0FFh // 判断是否小于FF
			mov value, eax	// 测试看看大小
			jl NotInVM	// 小于0xFF bRet返回0
			mov bRet,1	//大于0xFF,bRet返回1
NotInVM:	
	}

	printf("time: %d\n", value);
	return bRet;

}

// 虚拟机中的MAC一般是VMware默认的网卡MAC地址前缀为“00-05-69，00-0C-29或者00-50-56”，
// 这前3节是由VMware分配的唯一标识符OUI，以供它的虚拟化适配器使用
// VC6.0没有更新的SDK不支持GetAdaptersInfo函数
BOOL AntiVM::IsInsideVmVare_By_MAC()
{
	BOOL bRet = FALSE;
	// 参数1的结构体
	PIP_ADAPTER_INFO tag_AdapterInfo = new IP_ADAPTER_INFO[sizeof(IP_ADAPTER_INFO)];
	PIP_ADAPTER_INFO lpTemp = tag_AdapterInfo;

	// 参数2的大小
	ULONG lLen = sizeof(IP_ADAPTER_INFO);

	GetAdaptersInfo(tag_AdapterInfo, &lLen);
	GetAdaptersInfo(tag_AdapterInfo, &lLen);

	// 循环获取机器上的所有网卡信息.
	while(lpTemp)
	{
		// 截取MAC描述前6个字符
		lpTemp->Description[6] = '\0';
		/*printf("MAC: %02X-%02X-%02X-%02X-%02X-%02X\r\n",
		lpTemp->Address[0],
		lpTemp->Address[1],
		lpTemp->Address[2],
		lpTemp->Address[3],
		lpTemp->Address[4],
		lpTemp->Address[5]);*/

		// 前6个字符如果为VMware，其实就已经证明在主机了，
		// 除非 虚拟机里装虚拟机
		if (strcmp(lpTemp->Description, "VMware") == 0)
		{
			if ((lpTemp->Address[0] == 0x00 && lpTemp->Address[1] == 0x0C && lpTemp->Address[2] == 0x29) ||
				(lpTemp->Address[0] == 0x00 && lpTemp->Address[1] == 0x05 && lpTemp->Address[2] == 0x69) ||
				(lpTemp->Address[0] == 0x00 && lpTemp->Address[1] == 0x50 && lpTemp->Address[2] == 0x56) )
			{
				bRet = FALSE;
				break;
			}
		}
		// 如果前六个不等于VMware
		else
		{
			//MAC如果等于下面3组，就在虚拟机
			if ((lpTemp->Address[0] == 0x00 && lpTemp->Address[1] == 0x0C && lpTemp->Address[2] == 0x29) ||
				(lpTemp->Address[0] == 0x00 && lpTemp->Address[1] == 0x05 && lpTemp->Address[2] == 0x69) ||
				(lpTemp->Address[0] == 0x00 && lpTemp->Address[1] == 0x50 && lpTemp->Address[2] == 0x56) )
			{
				bRet = TRUE;
				break;
			}
		}


		lpTemp = lpTemp->Next;
	}


	delete[] tag_AdapterInfo;
	return bRet;
}

// 原理：查找注册表中HKEY_LOCAL_MACHINE\software\VMWare, Inc.\VMware tools项
BOOL AntiVM::IsInsideVmVare_By_Reg()
{
	BOOL bRet = FALSE;
	TCHAR szVMTools[] = _T("software\\VMWare, Inc.\\VMware tools");
	HKEY hSubKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szVMTools, 0, KEY_QUERY_VALUE, &hSubKey) == ERROR_SUCCESS)
	{
		bRet = TRUE;
	}
	return bRet;
}

// 原理：通过进程名vmtoolsd.exe查找
BOOL AntiVM::IsInsideVmVare_By_Process()
{
	BOOL bRet = FALSE;


	TCHAR szVMTool[] = _T("vmtoolsd.exe");
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnap)
	{
		::MessageBox(NULL, _T("创建进程失败"), _T("错误"), MB_ICONERROR);
		return FALSE;
	}
	if (Process32First(hSnap, &pe))
	{
		do
		{
			if (lstrcmpi(szVMTool, pe.szExeFile) == 0)
			{
				bRet = TRUE;
				break;
			}
		} while (Process32Next(hSnap, &pe));
	}

	CloseHandle(hSnap);
	return bRet;

}

// 原理：通过STR指令获取STR基址
// 测试发现：28000000为虚拟机 40000000为物理真机，可能有出入
BOOL AntiVM::IsInsideVmVare_By_STR()
{
	BOOL bRet = FALSE;
	unsigned char arr[4] = {0};

	__asm str arr;

	printf ("STR base: ");
	for (int i = 0; i < 4; i++)
	{
		printf("%02X", arr[i]);
	}
	puts("");
	if (arr[0] == 0x28 && arr[1] == 0x00)
	{
		bRet = TRUE;
	}
	return bRet;
}
//此方法已经失效，在VMware LDT的基址也变成了和主机一样的0x00000000
BOOL AntiVM::IsInsideVmVare_By_LDT()
{
	BOOL bRet = FALSE;
	unsigned char arr[4] = {0};
	__asm sldt arr;
	printf ("LDT base: ");
	for (int i = 0; i < 4; i++)
	{
		printf("%02X", arr[i]);
	}
	puts("");
	if (arr[0] == 0x00 && arr[1] == 0x00)
	{
		bRet = TRUE;
	}
	return bRet;
}
// 原理：当GDT基址位于0xFFXXXXXX时说明处于虚拟机中，否则为真实主机
BOOL AntiVM::IsInsideVmVare_By_GDT()
{
	BOOL bRet = FALSE;
	unsigned char arr[4] = {0};

	__asm sgdt arr;
	printf ("GDT base: ");
	for (int i = 0; i < 4; i++)
	{
		printf("%02X", arr[i]);
	}
	puts(""); // 换行
	if (arr[0] == 0xff)
	{
		bRet = TRUE;
	}
	return bRet;
}