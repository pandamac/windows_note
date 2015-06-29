#include <stdio.h>    
#include <tchar.h>      
#include <Shlwapi.h>      
#include <Psapi.h>    
#pragma comment(lib,"psapi")    
#define ARRAY_SIZE 1024    
       
int _tmain(int argc, _TCHAR* argv[])   
{                 
    LPVOID drivers[ARRAY_SIZE];    
    DWORD cbNeeded;    
    int cDrivers, i;    
       
    if (EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded)                
        && cbNeeded < sizeof(drivers))    
    {       
        CHAR szDriver[ARRAY_SIZE];        
        CHAR szPath[ARRAY_SIZE];    
        CHAR szDbgHelp[ARRAY_SIZE];    
        CHAR szSystem[ARRAY_SIZE];    
        size_t len = 0;       
        BOOL file = false;       
       
        GetSystemDirectory(szSystem, sizeof(szSystem));   
        strcat_s(szSystem, "\\dbghelp.dll");   
       
        cDrivers = cbNeeded / sizeof(drivers[0]);   
        for (i = 0; i < cDrivers; i++)   
        {   
            if (GetDeviceDriverBaseName(drivers[i], szDriver, sizeof(szDriver) / sizeof(szDriver[0])))   
            {   
                GetDeviceDriverFileName(drivers[i], szPath, sizeof(szPath));   
                if (szPath[1] == '?')   
                {   
                    len = strlen(szPath);   
                    do{   
                            len--;   
                        } while (szPath[len] != '\\');   
                        do{   
                                len--;   
                            } while (szPath[len] != '\\');   
                            szPath[len + 1] = 0;   
                            for (UINT i = 0; i < len; i++)   
                            {     
                                szPath[i] = szPath[i + 4];   
                            }   
                            sprintf_s(szDbgHelp, "%sdbghelp.dll", szPath);   
                            if (PathFileExists(szDbgHelp))   
                            {   
                                if (strcmp(szSystem, szDbgHelp) == 0)   
                                {   
                                    continue;   
                                }   
                                printf_s("OllyDbg Detected:\n"   
                                    "Path:%s\n"   
                                    "StrongOD Driver Name:%s\n", szPath, szDriver);   
                            }   
                        }   
                    }   
                }   
            }   
        return 0;   
}      