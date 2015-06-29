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

    HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE | SEC_IMAGE, 0, 0, NULL);
    DWORD dwImageBase = (DWORD)MapViewOfFile(hMap, FILE_MAP_READ | FILE_MAP_WRITE, NULL, NULL, 0);
    // 得到当前基地址
    //DWORD dwImageBase = (DWORD)GetModuleHandle(NULL);
    // 得到DOS头
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)dwImageBase;
    // 得到NT头
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(dwImageBase + pDosHeader->e_lfanew);
    // 得到IID头
    PIMAGE_IMPORT_DESCRIPTOR pIID = (PIMAGE_IMPORT_DESCRIPTOR)(dwImageBase + \
        pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    PIMAGE_THUNK_DATA pThunk = NULL;

    printf("-----------Import Table----------");
    while (pIID->Name)
    {
        DWORD n = 0;
        printf("----------------------------------\n");
        printf("DllName: %s\n", pIID->Name + dwImageBase);

        // 有些程序的OriginalFirstThunk为0，所以此时要用到FirstThunk
        if (pIID->OriginalFirstThunk)
        {
            pThunk = (PIMAGE_THUNK_DATA)(dwImageBase + pIID->OriginalFirstThunk);
        }
        else
        {
            pThunk = (PIMAGE_THUNK_DATA)(dwImageBase + pIID->FirstThunk);
        }

        while( *(DWORD*)pThunk)
        {

            // 判断最高位是否为1
            // 如果是1，表示函数以序号方式输入
            // 此时，低31位被看作是函数序号
            if (pThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG32)
            {
                // ul是联合体，随便哪个，值肯定只有一个，只是表示意义不同
                // 像MFC的模块就是以序号输入，只需要和FFFF &运算
                // 因为ul.Ordinal的值都是类似80001D56的值，只有第一位为1，第二三四位都为0
                // 为了程序的可读性，还是写成7FFFFFFFh
                printf("Ordinal = %08X \r\n", pThunk->u1.Ordinal & 0x7FFFFFFF);
            }
            else
            {
                PIMAGE_IMPORT_BY_NAME pFuncName = (PIMAGE_IMPORT_BY_NAME)(pThunk->u1.Function);
                printf("FuncName: %s\n", dwImageBase + pFuncName->Name);
                // 得到IAT的地址，这里必须通过ImageBase
                printf("Addr = %08X\n", pNtHeader->OptionalHeader.ImageBase + pIID->FirstThunk + n);
            }
            pThunk++;
            n += 4;
        }
        pIID++;
    }

    CloseHandle(hFile);
    UnmapViewOfFile((LPVOID)dwImageBase);
    return 0;
}