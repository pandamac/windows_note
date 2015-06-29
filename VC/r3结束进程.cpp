1.模拟按键alt+f4:
首先得设置被关闭窗口置顶，然后模拟按键alt+f4

HWND hwin = FindWindow(NULL,"notepad");

SetForegroundWindow(hwin); 

keybd_event(18,MapVirtualKey(18,0),0,0); 
keybd_event(115,MapVirtualKey(115,0),0,0); 
keybd_event(115,MapVirtualKey(115,0),KEYEVENTF_KEYUP,0); 
keybd_event(115,MapVirtualKey(115,0),0,0); 
keybd_event(115,MapVirtualKey(115,0),KEYEVENTF_KEYUP,0); 
keybd_event(18,MapVirtualKey(18,0),KEYEVENTF_KEYUP,0); 
2.发送WM_CLOSE消息关闭窗口

HWND hWin = FindWindow(NULL, "notepad");
SendMessage(hWin, WM_CLOSE, 0, 0);
return 0;

3.调用 TerminateProcess(HANDLE, ExitCode)结束进程,TerminateThread(HANDLE, ExitCode)结束线程

4.找到进程的线程，PostThraedMessage 发送

上面的两种方法首先都要获得进程句柄，或者线程句柄
进程句柄的获取
a.知道进程pid: HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
b.知道进程名（notepad）:

1）使用ToolHelp来获取
2）使用PSAPI来获取
3) 使用 ntdll.dll 中的未文档化的 NtQuerySystemInformation 之类的 API 来获取
使用ToolHelp获取：
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>

DWORD WINAPI GetPIDbyName(LPTSTR lpName)
{
    HANDLE m_Snap = INVALID_HANDLE_VALUE;
    PROCESSENTRY32 pe = {sizeof(pe)};
    DWORD ret = NULL;


    m_Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,NULL);
    if (m_Snap == INVALID_HANDLE_VALUE) goto finally; 
    if (!Process32First(m_Snap, &pe)) goto finally;
    do 
    {
        if(!lstrcmpi(pe.szExeFile,lpName)) 
        {
           ret = pe.th32ProcessID;
           goto finally;
        }
    }while (Process32Next(m_Snap, &pe)); 
}

finally: {
if (m_Snap != INVALID_HANDLE_VALUE) 
CloseHandle(m_Snap);
}
return ret;
}

int main(int argc, char *argv[])
{
   DWORD Pid = 0;
   Pid = GetPIDbyName("notepad.exe");
   if(Pid <= 0)
      return -1;
}
同样获取线程的PID也可以使用上面的方法，然后调用OpenThread 获取线程句柄