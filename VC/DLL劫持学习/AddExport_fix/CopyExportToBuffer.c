#include <WINDOWS.H>
#include "AddExport.h"

#define NFIND_ORD 0x0;
#define FIND_ORD 0x1;

/*
参数：
lpFile指向内存映射文件
lpExport指向内存映射文件的导出表
lpBuffer指向需要将内存映射文件的导出表附带内容拷贝到的内存区域，该内存区域既是新的PE文件内存映射的导出表地址
lpTransitName指向中转DLL名
Rva为新的PE文件的内存映射的导出表地址的Rva，也就是lpBuffer的Rva
功能：将lpFile指向的内存映射文件的导出表附带所有信息拷贝到新文件的内存映射中
返回值：返回NULL为出错，否则返回FALSE
*/

BOOL WINAPI CopyExportToBuffer(PIMAGE_DOS_HEADER lpFile,DWORD lpExport,DWORD lpBuffer,LPSTR lpTransitName,DWORD Rva)
{
	PIMAGE_EXPORT_DIRECTORY lpExp;	//保存指向导出表的指针
	PIMAGE_EXPORT_DIRECTORY lpBuff;	//保存新导出表的指针
	PCHAR lpTmp;	//临时指针变量，指向下一个需要读写的lpBuffer
	DWORD *lpFunRva;	//存放指向新文件的函数地址Rva数组指针
	DWORD *lpNameRva;	//存放指向新文件的函数名字Rva数组的指针
	WORD *lpNameOrd;	//存放指向新文件的函数索引数组指针

	DWORD *lpOldFunRva;	//存放指向被拷贝文件的函数地址Rva数组指针
	DWORD *lpOldNameRva;	//存放指向被拷贝文件的函数名字Rva数组的指针
	WORD *lpOldNameOrd;	//存放指向被拷贝文件的函数索引数组指针
	DWORD lpName;	//临时存放指向字符串的指针
	DWORD nBase;	//存放被拷贝文件的导出函数起始序号
	DWORD i;
	DWORD m;
	DWORD f;
//	static char szFormat[] = "Bommon.%s";
//	static char szFormatOrd[] = "Bommon.#%d";
	static char szFormat[] = "%.259s.%.259s";	//格式化otherdll.exportname
	static char szFormatOrd[] = "%.259s.#%d";
	static char szBuffer[MAX_PATH * 2 + 2];

//	wsprintfA(szBuffer1,szFormat,lpTransitName);
//	wsprintfA(szBuffer2,szFormatOrd,lpTransitName);

	lpBuff = lpBuffer;
	lpExp = lpExport;
	//拷贝三个结构成员
	lpBuff->Base = lpExp->Base;
	lpBuff->NumberOfFunctions = lpExp->NumberOfFunctions;
	lpBuff->NumberOfNames = lpExp->NumberOfNames;
	//预留AddressOfFunctions、AddressOfNames、AddressOfNameOrdinals空间
	lpTmp = lpBuff;		//将新导出表地址赋予临时指针变量
	lpTmp += sizeof (IMAGE_EXPORT_DIRECTORY);	//移动临时指针保留导出表结构所需空间
	lpFunRva = lpTmp;	//保存指向函数地址的Rva数组指针
	lpBuff->AddressOfFunctions = lpTmp-lpBuff+Rva;	//保存计算出来的Rva，AddressOfFunctions
	lpTmp += ((lpExp->NumberOfFunctions)*(sizeof (DWORD)));	//移动指针保留函数地址Rva数组所需空间
	lpNameRva = lpTmp;	//保存指向函数名字的Rva数组指针
	lpBuff->AddressOfNames = lpTmp-lpBuff+Rva;	//保存计算出来的Rva，AddressOfNames
	lpTmp += ((lpExp->NumberOfNames)*(sizeof (DWORD)));	//移动指针保留函数名字Rva数组所需空间
	lpNameOrd = lpTmp;	//保存函数序号的数组指针
	lpBuff->AddressOfNameOrdinals = lpTmp-lpBuff+Rva;	//保存计算出来的Rva，AddressOfNameOrdinals
	lpTmp += (lpExp->NumberOfNames)*(sizeof (WORD));	//移动指针保留函数序号数组所需空间

	//得到原始函数地址数组等的offset
	lpOldFunRva = RvaToOffset(lpFile,lpExp->AddressOfFunctions);
	lpOldNameRva = RvaToOffset(lpFile,lpExp->AddressOfNames);
	lpOldNameOrd = RvaToOffset(lpFile,lpExp->AddressOfNameOrdinals);
	nBase = lpExp->Base;
	//循环拷贝函数入口地址Rva，该Rva指向中转字符串，Dll.Name1
	//若有只能以导出序号导出的函数，则将导出函数Rva指向DllName.#19这种形式的中转字符串
	for (i=0;lpExp->NumberOfFunctions > i;i++)	//以所有函数数目作为外层循环次数
	{
		f = NFIND_ORD;	//设置标志，NFIND_ORD为未找到函数名字对应的Ord，也就是只能以序号导出的函数，反之
		for (m=0;m < lpExp->NumberOfNames;m++)
		{
			if (i==lpOldNameOrd[m])
			{
				f = FIND_ORD;
				break;
			}
		}
		if (!f)
		{
			//生成序号字符串
			lpFunRva[i] = lpTmp-lpBuff+Rva;
			wsprintfA(szBuffer,szFormatOrd,lpTransitName,i+nBase);
			lstrcpyA(lpTmp,szBuffer);
			lpTmp += lstrlenA(szBuffer);
			lpTmp++;
			continue;
		}

		//生成中转字符串
		lpFunRva[i] = lpTmp-lpBuff+Rva;
		lpName = RvaToOffset(lpFile,lpOldNameRva[m]);
		wsprintfA(szBuffer,szFormat,lpTransitName,lpName);
		lstrcpyA(lpTmp,szBuffer);
		lpTmp += lstrlenA(szBuffer);
		lpTmp++;
	}
	
	//拷贝原始函数名，并填写Rva
	for (i=0;lpExp->NumberOfNames > i;i++)
	{
		lpNameRva[i] = lpTmp-lpBuff+Rva;
		lpName = RvaToOffset(lpFile,lpOldNameRva[i]);
		//wsprintfA(szBuffer,szFormat,lpName);
		lstrcpyA(lpTmp,lpName);
		lpTmp += lstrlenA(lpName);
		lpTmp++;
	}

	//拷贝NameOrdinals数组
	for (i=0;lpExp->NumberOfNames > i;i++)
		lpNameOrd[i] = lpOldNameOrd[i];
	//拷贝原始DLL名字
	lpName = RvaToOffset(lpFile,lpExp->Name);
	lstrcpyA(lpTmp,lpName);
	lpBuff->Name = lpTmp-lpBuff+Rva;

	return TRUE;
}
