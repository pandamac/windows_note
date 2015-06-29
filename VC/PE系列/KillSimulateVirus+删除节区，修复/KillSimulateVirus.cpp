// KillSimulateVirus.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tchar.h>
#include <Windows.h>
#include <stddef.h>


//扫描到文件时调用的回调函数类型
enum VIRUS_DEAL_TYPE{NOT_PE, PE_NOT_VIRUS, INFECT_HEADSTREAM_DELETED, CARRY_VIRUS_CLEARED, VIRUS_FAILED};
typedef VIRUS_DEAL_TYPE (*ScanFileCallBack)(TCHAR *pFileName, PVOID pThis);

void ScanDirectory(TCHAR *pStrDirectory, ScanFileCallBack funcDoSomething, PVOID pThis = NULL)
{
	if(pStrDirectory == NULL)
	{
		return ;
	}
	WIN32_FIND_DATA fData = {0};
	TCHAR tcDirectory[MAX_PATH] = {0}, tcScanFileType[MAX_PATH] = {0};
	int iLength = _tcslen(pStrDirectory);
	if (iLength > MAX_PATH) //如果目录路径过长则失败
	{
		return ;
	}
	_tcscpy(tcScanFileType, pStrDirectory);  //保存扫描文件夹
	
	if (_T('\\') == tcScanFileType[iLength - 1])
		_tcscat(tcScanFileType, _T("*.*"));
	else
		_tcscat(tcScanFileType, _T("\\*.*")); //构造扫描文件类型名
	HANDLE hFindHandle = FindFirstFile(tcScanFileType, &fData);
	if (hFindHandle == INVALID_HANDLE_VALUE)
	{
		return;
	}
	do 
	{
		if (_T('.') == fData.cFileName[0])
		{   //跳过上层目录
			continue;
		}
		_tcscpy(tcDirectory, pStrDirectory);  //保存扫描文件夹
		if ( '\\' != tcDirectory[iLength - 1])
			_tcscat(tcDirectory, _T("\\"));
		
		if (iLength + strlen(fData.cFileName) > MAX_PATH) //文件名过长则失败
		{
			return;
		}
		strcat(tcDirectory, fData.cFileName); //新文件名或子文件夹名
		fData.dwFileAttributes &= FILE_ATTRIBUTE_DIRECTORY;
		if (FILE_ATTRIBUTE_DIRECTORY == fData.dwFileAttributes)//判断是否是目录
		{
			ScanDirectory(tcDirectory, funcDoSomething, pThis);  //枚举子目录
		}
		else
		{
			VIRUS_DEAL_TYPE virusDealRet = funcDoSomething(tcDirectory, pThis);   //目录中的文件,执行相应函数。
			TCHAR tcBuffer[128] = {0};
			switch (virusDealRet)
			{
			case NOT_PE:
				_tcscpy(tcBuffer, "非PE文件");
				break;
			case PE_NOT_VIRUS:
				_tcscpy(tcBuffer, "不是病毒");
				break;
			case INFECT_HEADSTREAM_DELETED:
				_tcscpy(tcBuffer, "感染源，删除成功");
				break;
			case CARRY_VIRUS_CLEARED:
				_tcscpy(tcBuffer, "病毒携带者，清除成功");
				break;
			case VIRUS_FAILED:
				_tcscpy(tcBuffer, "病毒，杀除失败");
				break;
			}
			printf("%-40s 扫描结果: %s\n", tcDirectory, tcBuffer);
		}
	} while (FindNextFile(hFindHandle, &fData));
	FindClose(hFindHandle);
}

//判断是否为PE结构
BOOL IsPeFile(TCHAR* pFileName)
{
	if (pFileName == NULL)
	{
		return FALSE;
	}
	//获得指定文件句柄
	HANDLE hFile = CreateFile(pFileName, GENERIC_READ , FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == NULL || hFile == INVALID_HANDLE_VALUE)
	{	
		return FALSE;
	}
	WORD dwTempRead;
	DWORD dwReadInFactSize;
	//读取文件开始两字节,即MZ头
	BOOL bRead = ReadFile(hFile, &dwTempRead, sizeof(WORD), &dwReadInFactSize, NULL);
	if (!bRead || sizeof(WORD) != dwReadInFactSize)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	if (dwTempRead != 0x5a4d)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	//得到e_lfanew 成员在IMAGE_DOS_HEADER 结构中的偏移
	DWORD dwSize = offsetof(IMAGE_DOS_HEADER, e_lfanew);
	//将文件指针移动到得到e_lfanew成员
	SetFilePointer(hFile, dwSize, NULL, FILE_BEGIN);
	//读取得到e_lfanew成员的内容,也就是PE头在文件中的偏移
	bRead = ReadFile(hFile, &dwTempRead, sizeof(WORD), &dwReadInFactSize, NULL);
	if (!bRead || sizeof(WORD) != dwReadInFactSize)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	//将指针移动到PE头
	SetFilePointer(hFile, dwTempRead, NULL, FILE_BEGIN);
	//读取PE标志
	IMAGE_NT_HEADERS NtHeader = {0};
	bRead = ReadFile(hFile, &NtHeader, sizeof(NtHeader), &dwReadInFactSize, NULL);
	if (!bRead || sizeof(NtHeader) != dwReadInFactSize)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	if (NtHeader.Signature != 0x4550)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	CloseHandle(hFile);
	//该文件属于PE格式,返回TRUE.
	return TRUE;
}

//将指定文件读取到内存
BOOL GetFileContent(TCHAR *pPath, DWORD &dwSize, PBYTE pMem = NULL)
{
	if (pPath == NULL)
	{
		return FALSE;
	}
	HANDLE hFile = CreateFile(pPath, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	dwSize = GetFileSize(hFile, NULL);
	if (pMem != NULL)
	{
		DWORD dwRead = 0;
		BOOL bRet = ReadFile(hFile, pMem, dwSize, &dwRead, NULL);
		if (!bRet || dwRead != dwSize)
		{
			CloseHandle(hFile);
			return FALSE;
		}
	}
	CloseHandle(hFile);
	return TRUE;
}

//将指定内存内容写入到文件
BOOL ContentToFile(TCHAR *pPath, PBYTE pMem, DWORD dwSize)
{
	if (pMem == NULL || pPath == NULL)
	{
		return FALSE;
	}
	HANDLE hFile = CreateFile(pPath, GENERIC_WRITE, FILE_SHARE_READ, 
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	DWORD dwWrite = 0;
	BOOL bRet = WriteFile(hFile, pMem, dwSize, &dwWrite, NULL);
	if (!bRet || dwWrite != dwSize)
	{
		int i = GetLastError();
		CloseHandle(hFile);
		return FALSE;
	}
	CloseHandle(hFile);
	return TRUE;
}

enum VIRUS_TYPE{NOT_VIRUS, INFECT_HEADSTREAM, CARRY_VIRUS_APPEND, 
CARRY_VIRUS_RESOURCE, CARRY_VIRUS_ADDSECTION};

VIRUS_TYPE JudgeFile(TCHAR *pFileName)
{
	VIRUS_TYPE virusType = NOT_VIRUS;
	DWORD dwSize = 0, dwSectionNum = 0, dwSizePe = 0;
	PBYTE pMem = NULL;
	IMAGE_DOS_HEADER *pPeDosHeader = NULL;
	IMAGE_NT_HEADERS *pPeHeader = NULL;
	IMAGE_SECTION_HEADER * pPeSectionHeader = NULL;
	//1、读取目标文件到内存
	BOOL bRet = GetFileContent(pFileName, dwSize);
	if (!bRet)
		goto Exit ;
	pMem = new BYTE[dwSize];
	if (pMem == NULL)
		goto Exit ;
	bRet = GetFileContent(pFileName, dwSize, pMem);
	if (!bRet)
		goto Exit ;
	
	//PE文件的Dos头
	pPeDosHeader = (IMAGE_DOS_HEADER *)pMem;
	//PE头在文件中的偏移：m_pPeDosHeader->e_lfanew
	pPeHeader = (IMAGE_NT_HEADERS *)((BYTE *)pMem + ((IMAGE_DOS_HEADER *)pMem)->e_lfanew);
	//PE文件中节的个数
	dwSectionNum = pPeHeader->FileHeader.NumberOfSections;
	//PE文件节表头在文件中的偏移
	pPeSectionHeader = (IMAGE_SECTION_HEADER *)((char *)&pPeHeader->OptionalHeader + 
		pPeHeader->FileHeader.SizeOfOptionalHeader);
	//求出有效PE的长度
	dwSizePe = pPeSectionHeader[dwSectionNum-1].SizeOfRawData+
		pPeSectionHeader[dwSectionNum-1].PointerToRawData;
	//2.进行判断,如果含有感染标记,则是病毒
	if (pPeDosHeader->e_cblp == 0x8F8F && pPeDosHeader->e_cs == 0x3e3e)
	{
		HMODULE hExe = LoadLibrary(pFileName);
		if (hExe == NULL)
		{
			goto Exit;
		}
		HRSRC hRsrc = FindResource(hExe, (LPCTSTR)"EXE", (LPCSTR)"BINARYDATA");
		if (hRsrc != NULL)
		{    //找到资源，则为资源捆绑式携带者
			virusType = CARRY_VIRUS_RESOURCE;
		}
		else if (dwSize > dwSizePe)
		{//如果文件实际大小大于PE文件有效大小，则是附加数据捆绑式携带者。
			virusType = CARRY_VIRUS_RESOURCE;
		}
		else
		{	//是感染源
			virusType = INFECT_HEADSTREAM;
		}
		FreeLibrary(hExe);
	}
	else if (pPeDosHeader->e_ip == 0xfeff && pPeDosHeader->e_cs == 0xfeff)
	{	//如果最后一个节是.boy
		if (strcmp((char *)pPeSectionHeader[dwSectionNum-1].Name, ".boy") == 0)
		{
			virusType = CARRY_VIRUS_ADDSECTION;
		}
		else
		{	//是感染源
			virusType = INFECT_HEADSTREAM;
		}
	}
Exit:
	if (pMem != NULL)
	{
		delete []pMem;
		pMem = NULL;
	}
	return virusType;
}

BOOL KillVirus(TCHAR *pFileName, VIRUS_TYPE virusType)
{
	BOOL bRet = FALSE;
	switch (virusType)
	{
	case INFECT_HEADSTREAM:
		{
			bRet = DeleteFile(pFileName);
		}
		break;
	case CARRY_VIRUS_APPEND:
		{	//将病毒携带者读取到内存
			DWORD dwSize = 0;
			bRet = GetFileContent(pFileName, dwSize);
			if (!bRet)
				break;
			PBYTE pMem = new BYTE[dwSize];
			if (pMem == NULL)
				break;
			bRet = GetFileContent(pFileName, dwSize, pMem);
			if (!bRet)
			{
				delete []pMem;
				break;
			}
			//PE文件的Dos头
			IMAGE_DOS_HEADER * pPeDosHeader = (IMAGE_DOS_HEADER *)pMem;
			//PE头在文件中的偏移：m_pPeDosHeader->e_lfanew
			IMAGE_NT_HEADERS * pPeHeader = (IMAGE_NT_HEADERS *)((BYTE *)pMem + ((IMAGE_DOS_HEADER *)pMem)->e_lfanew);
			//PE文件中节的个数
			DWORD dwSectionNum = pPeHeader->FileHeader.NumberOfSections;
			//PE文件节表头在文件中的偏移
			IMAGE_SECTION_HEADER * pPeSectionHeader = (IMAGE_SECTION_HEADER *)((char *)&pPeHeader->OptionalHeader + 
				pPeHeader->FileHeader.SizeOfOptionalHeader);
			//求出有效PE的长度
			DWORD dwSizePe = pPeSectionHeader[dwSectionNum-1].SizeOfRawData+
			pPeSectionHeader[dwSectionNum-1].PointerToRawData;
			bRet = DeleteFile(pFileName);
			if (!bRet)
			{
				delete []pMem;
				break;
			}
			//将病毒携带者附加数据中的文件释放出来
			bRet = ContentToFile(pFileName, pMem+dwSizePe, dwSize-dwSizePe);
		}
		break;
	case CARRY_VIRUS_RESOURCE:
		{
			HMODULE hExe = LoadLibrary(pFileName);
			if (hExe == NULL)
			{
				return bRet;
			}
			HRSRC hRsrc = FindResource(hExe, (LPCTSTR)"EXE", (LPCSTR)"BINARYDATA");
			int iError = 0;
			if (hRsrc == NULL)
			{
				return bRet; 
			}
			HGLOBAL hData = LoadResource(hExe, hRsrc);
			if (hData == NULL)
				return bRet;
			LPVOID pMem = LockResource(hData);
			if (pMem == NULL)
				return bRet;
			int iSize = SizeofResource(hExe, hRsrc);
			PBYTE pTemp = new BYTE[iSize];
			if (pTemp == NULL)
				return bRet;
			memcpy(pTemp, pMem, iSize);
			FreeLibrary(hExe);
			bRet = DeleteFile(pFileName);
			//将病毒携带者附加数据中的文件释放出来
			bRet = ContentToFile(pFileName, (PBYTE)pTemp, iSize);
			delete []pTemp;
		}
		break;
	case CARRY_VIRUS_ADDSECTION:
		{
			//将病毒携带者读取到内存
			DWORD dwSize = 0;
			bRet = GetFileContent(pFileName, dwSize);
			if (!bRet)
				break;
			PBYTE pMem = new BYTE[dwSize];
			if (pMem == NULL)
				break;
			bRet = GetFileContent(pFileName, dwSize, pMem);
			if (!bRet)
			{
				delete []pMem;
				break;
			}
			//PE文件的Dos头
			IMAGE_DOS_HEADER * pPeDosHeader = (IMAGE_DOS_HEADER *)pMem;
			//PE头在文件中的偏移：m_pPeDosHeader->e_lfanew
			IMAGE_NT_HEADERS * pPeHeader = (IMAGE_NT_HEADERS *)((BYTE *)pMem + ((IMAGE_DOS_HEADER *)pMem)->e_lfanew);
			//PE文件中节的个数
			DWORD dwSectionNum = pPeHeader->FileHeader.NumberOfSections;
			//PE文件节表头在文件中的偏移
			IMAGE_SECTION_HEADER * pPeSectionHeader = (IMAGE_SECTION_HEADER *)((char *)&pPeHeader->OptionalHeader + 
				pPeHeader->FileHeader.SizeOfOptionalHeader);
			//求出有效PE的长度
			DWORD dwSizePe = pPeSectionHeader[dwSectionNum-1].SizeOfRawData+
			pPeSectionHeader[dwSectionNum-1].PointerToRawData;
			int i = 0;
			//获得最后一节所在内存地址
			PBYTE pLastSecPtr = NULL;
			for (; i < dwSectionNum; i++)
			{
				if (strcmp((char *)pPeSectionHeader[i].Name, ".boy") == 0)
				{
					break;
				}
			}
			if (i != 0)
			{
				pLastSecPtr = (PBYTE)pMem + pPeSectionHeader[i].PointerToRawData;
			}
			//取出节偏移4字节后保存的原始入口地址，得到其RVA值。
			DWORD dwOep = *(PDWORD((PDWORD)pLastSecPtr+1))-pPeHeader->OptionalHeader.ImageBase;
			//清除感染标记
			*(DWORD *)(pMem + 20) = 0x00000000;
			//恢复OEP
			pPeHeader->OptionalHeader.AddressOfEntryPoint = dwOep;
			//修正镜像大小。
			pPeHeader->OptionalHeader.SizeOfImage -= pPeSectionHeader[dwSectionNum-1].Misc.VirtualSize;;
			//删除最后一个节
			pPeHeader->FileHeader.NumberOfSections--;
			memset(&pPeSectionHeader[dwSectionNum-1], 0, sizeof(IMAGE_SECTION_HEADER));
			//删除病毒携带者
			bRet = DeleteFile(pFileName);
			//将还原后的文件保存
			bRet = ContentToFile(pFileName, pMem, dwSize-pPeSectionHeader[dwSectionNum-1].SizeOfRawData);
		}
		break;
	}
	return bRet;
}
VIRUS_DEAL_TYPE DealWith(TCHAR *pFileName, PVOID pPara)
{
	VIRUS_DEAL_TYPE virusDealType = PE_NOT_VIRUS;
	if (IsPeFile(pFileName)) //首先判断是否为PE文件
	{
		VIRUS_TYPE virusType = JudgeFile(pFileName); //判断病毒类型
		if (virusType != NOT_VIRUS)
		{	//如果是病毒则进行杀毒
			BOOL bRet = KillVirus(pFileName, virusType);
			if (bRet && virusType == INFECT_HEADSTREAM)
			{	//成功删除感染源
				virusDealType = INFECT_HEADSTREAM_DELETED;
			} 
			else if (bRet && (virusType == CARRY_VIRUS_APPEND || 
				virusType == CARRY_VIRUS_RESOURCE || virusType == CARRY_VIRUS_ADDSECTION))
			{	//成功清除携带者中的病毒
				virusDealType = CARRY_VIRUS_CLEARED;
			} 
			else
			{	//杀毒失败
				virusDealType = VIRUS_FAILED;
			}
		}
	}
	else
	{	//不是有效的PE文件
		virusDealType = NOT_PE;
	}
	return virusDealType;
}

int main(int argc, char* argv[])
{
	ScanDirectory(argv[1], DealWith, NULL);
	return 0;
}

