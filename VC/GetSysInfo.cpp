#include <windows.h>
#include <stdio.h>

void GetSysInfo()
{
    char szComputerName[MAXBYTE] = { 0 };
    char szUserName[MAXBYTE] = { 0 };
    unsigned long nSize = MAXBYTE;
	
    OSVERSIONINFO OsVer; 
	
    OsVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&OsVer);

    if ( OsVer.dwPlatformId == VER_PLATFORM_WIN32_NT )
    {
        if ( OsVer.dwMajorVersion == 5 && OsVer.dwMinorVersion == 1 )
        {
            printf("Windows XP %s \r\n", OsVer.szCSDVersion);
        }
        else if ( OsVer.dwMajorVersion == 5 && OsVer.dwMinorVersion == 0)
        {
            printf("Windows 2K \r\n");
        }
    }
    else
    {
        printf("Ohter System \r\n");
    }

    GetComputerName(szComputerName, &nSize);

    printf("Computer Name is %s \r\n", szComputerName);

    nSize = MAXBYTE;
    GetUserName(szUserName, &nSize);

    printf("User Name is %s \r\n", szUserName);
}

int main()
{
    GetSysInfo();
    return 0;
}