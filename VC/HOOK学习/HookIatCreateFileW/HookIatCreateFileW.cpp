#include <Windows.h>

typedef HANDLE (WINAPI *CREATEFILEW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

CREATEFILEW dwCreateFileWAddr = 0;

HANDLE
WINAPI
MyCreateFileW(
            LPCWSTR lpFileName,
            DWORD dwDesiredAccess,
            DWORD dwShareMode,
            LPSECURITY_ATTRIBUTES lpSecurityAttributes,
            DWORD dwCreationDisposition,
            DWORD dwFlagsAndAttributes,
            HANDLE hTemplateFile
    )
{
    WCHAR wFileName[MAX_PATH] = { 0 };
    wcscpy(wFileName, lpFileName);
    if ( wcscmp(wcslwr(wFileName), L"g:\\test.txt") == 0 )
    {
        if ( MessageBox(NULL, "是否打开文件", "提示", MB_YESNO) == IDYES )
        {
            return dwCreateFileWAddr(lpFileName,
                            dwDesiredAccess,
                            dwShareMode,
                            lpSecurityAttributes,
                            dwCreationDisposition,
                            dwFlagsAndAttributes,
                            hTemplateFile);
        } 
        else
        {
            return INVALID_HANDLE_VALUE;
        }
    }
    else
    {
        return dwCreateFileWAddr(lpFileName,
            dwDesiredAccess,
            dwShareMode,
            lpSecurityAttributes,
            dwCreationDisposition,
            dwFlagsAndAttributes,
            hTemplateFile);
    }
}

VOID HookNotePadProcessIAT()
{
    // 获得Createfile
    HMODULE hMod = LoadLibrary("kernel32.dll");
    DWORD dwFuncAddr = (DWORD)GetProcAddress(hMod, "CreateFileW");
    CloseHandle(hMod);

    // 获取记事本进程模块基址
    HMODULE hModule = GetModuleHandleA(NULL);

    // 定位PE结构
    PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)((DWORD)hModule + pDosHdr->e_lfanew);

    // 保存映像基址及导入表的RVA
    DWORD dwImageBase = pNtHdr->OptionalHeader.ImageBase;
    DWORD dwImpRva = pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

    // 导入表的VA
    PIMAGE_IMPORT_DESCRIPTOR pImgDes = (PIMAGE_IMPORT_DESCRIPTOR)(dwImageBase + dwImpRva);

    char szAddr[10] = { 0 };

    PIMAGE_IMPORT_DESCRIPTOR pTmpImpDes = pImgDes;
    BOOL bFound = FALSE;

    // 查找欲HOOK函数的模块名
    while ( pTmpImpDes->Name )
    {
        DWORD dwNameAddr = dwImageBase + pTmpImpDes->Name;
        char szName[MAXBYTE] = { 0 };
        strcpy(szName, (char*)dwNameAddr);

        if ( strcmp(strlwr(szName), "kernel32.dll") == 0 )
        {
            bFound = TRUE;
            break;
        }
        pTmpImpDes ++;
    }

    // 判断是否找到欲HOOK函数所在的函数名
    if ( bFound == TRUE )
    {
        bFound = FALSE;
        char szAddr[10] = { 0 };

        // 逐个遍历该模块的IAT地址
        PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)(pTmpImpDes->FirstThunk + dwImageBase);

        while ( pThunk->u1.Function )
        {
            DWORD *pAddr = (DWORD *)&(pThunk->u1.Function);
            // 比较是否与欲HOOK函数的地址相同
            if ( *pAddr == dwFuncAddr )
            {
                bFound = TRUE;
                dwCreateFileWAddr = (CREATEFILEW)*pAddr;
                DWORD dwMyHookAddr = (DWORD)MyCreateFileW;
                // 修改为Hook函数的地址
                WriteProcessMemory(GetCurrentProcess(), (LPVOID)pAddr, &dwMyHookAddr, sizeof(DWORD), NULL);
                break;
            }
            pThunk ++;
        }
    }

    if ( bFound == TRUE )
    {
        MessageBox(NULL, "Hook Successfully !", NULL, MB_OK);
    } 
    else
    {
        MessageBox(NULL, "Hook Unsuccessfully !", NULL, MB_OK);
    }
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch ( ul_reason_for_call )
    {
    case DLL_PROCESS_ATTACH:
        {
            // 在DLL被加载时调用HookNotePadProcessIAT()
            HookNotePadProcessIAT();
            break;
        }
    }
    return TRUE;
}