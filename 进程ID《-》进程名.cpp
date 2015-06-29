TCHAR* GetProcessName(DWORD processID )
{
 HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

 TCHAR* procName = new TCHAR[MAX_PATH];

 GetModuleFileNameEx(hProcess, NULL, procName, MAX_PATH);

 return procName;
}

DWORD WINAPI GetProcessIdByName(LPCTSTR lpFilename)
{
    DWORD dwProcessId = 0;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(hProcessSnap == INVALID_HANDLE_VALUE) return 0;
    if(!Process32First(hProcessSnap,&pe32)){CloseHandle(hProcessSnap);return 0;}
     
    do
    {
        _strupr(pe32.szExeFile);
        if(strstr(pe32.szExeFile,szFilename))
        {
            dwProcessId = pe32.th32ProcessID;
            break;
        }
    }
    while(Process32Next(hProcessSnap,&pe32));
     
    CloseHandle(hProcessSnap);
     
    return dwProcessId;
}
