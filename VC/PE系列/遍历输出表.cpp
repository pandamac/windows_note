#include "stdafx.h"
#include <Windows.h>

int main(int argc, char *argv[])
{
    HANDLE hFile = CreateFile(
        argv[1], 
        GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    DWORD dwFileSize = GetFileSize(hFile, NULL);
    HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE | SEC_IMAGE, 0, dwFileSize, NULL);
    if (hMap == NULL)
    {
        CloseHandle(hFile);
        return -1;
    }
    DWORD dwImageBase = (DWORD)MapViewOfFile(hMap, FILE_MAP_READ | FILE_MAP_WRITE, NULL, NULL, dwFileSize);
    if (dwImageBase==NULL)
    {
        CloseHandle(hFile);
        CloseHandle(hMap);
        return -1;
    }
    // 得到当前基地址
    //DWORD dwImageBase = (DWORD)GetModuleHandle(NULL);
    // 得到DOS头
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)dwImageBase;
    // 得到NT头
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(dwImageBase + pDosHeader->e_lfanew);
    // 得到IID头

    PIMAGE_EXPORT_DIRECTORY pExportDir = (PIMAGE_EXPORT_DIRECTORY)(dwImageBase + \
        pNtHeader->OptionalHeader.DataDirectory[0].VirtualAddress);


    int nFuncNum = pExportDir->NumberOfFunctions; // 为4
    int nBase = pExportDir->Base; // nBase = 2; 2  3  4  5
    int iNameOrdinalsIndex = -1; 

    // 导出序号表首地址（WORD大小）
    WORD *pNameOrdinalsTable = (WORD *)(dwImageBase + pExportDir->AddressOfNameOrdinals);

    // 导出函数名称数组首地址（设置为DWORD数组）
    DWORD *pNameAddress = (DWORD *)(pExportDir->AddressOfNames + dwImageBase);

    // EAT数组首地址
    DWORD *pRVAFunc = (DWORD *)(pExportDir->AddressOfFunctions + dwImageBase);

    printf("----------------Export Table  Start---------------------\n");
    printf("\n");
    printf("DllName: %s\n", dwImageBase + pExportDir->Name);

    for (int i = 0; i < nFuncNum; i++) 
    {
        BOOL bFind = FALSE;

        for (int j = 0; j < (int)pExportDir->NumberOfNames; j++) 
        {
            // 导出序号 + base基数
            int iIndex = pNameOrdinalsTable[j] + nBase;
            // 如果能找到，就表示以名称和序号，否则只是以序号
            if (iIndex == i + nBase)
            {
                iNameOrdinalsIndex++;
                bFind = TRUE; // 名称+序号
                break;
            }
        }

        if (bFind)
        {
            // 打印函数名字
            printf("序号：%4d\t", i+nBase);
            printf("RVA: 0x%08X\t", pRVAFunc[i]);
            printf("FuncName: %s\n", pNameAddress[iNameOrdinalsIndex] + dwImageBase);
        }

        // EAT中RVA为0就不输出
        else if(pRVAFunc[i])
        {
            printf("序号：%4d\t", i+nBase);
            printf("RVA: 0x%08X\t", pRVAFunc[i]);
            printf("FuncName: --\n");
        }
    }

    puts("");
    printf("----------------Export Table  End---------------------\n");
    CloseHandle(hFile);
    CloseHandle(hMap);
    UnmapViewOfFile((LPVOID)dwImageBase);

    return 0;
}