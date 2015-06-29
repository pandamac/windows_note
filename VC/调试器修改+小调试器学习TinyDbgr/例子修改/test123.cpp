#include "stdio.h"
#include "tchar.h"
#include "windows.h"

int _tmain(int argc, _TCHAR* argv[])
{
	STARTUPINFO stSi;
	PROCESS_INFORMATION stPi;
	DEBUG_EVENT stDe;
	int nAddrOfBreakPoint = 0x0040E7F3; //需要下断点的地址
	//UPX   0040E7F3   .- E9 912BFFFF   jmp mengmeng.00401389 

	int nAddrOfPatch = 0x004010B8;    //需要打补丁处的地址
	unsigned char cOldByte;        //用于保存nAddrOfBreakPoint断点处的原指令
	unsigned char cBreakPoint = 0xCC;  //int 3指令
	unsigned char cPatchByte = 0x75;  //补丁字节
    CONTEXT stThreadContext;
	BOOL bCreated = FALSE;
	BOOL bFinished = FALSE;

	//LPTSTR szCmdLine = _tcsdup(TEXT("mengmeng.exe"));
	LPTSTR szCmdLine = _T("mengmeng.exe");
	GetStartupInfo(&stSi);
	bCreated = CreateProcess(NULL,szCmdLine,NULL,NULL,FALSE,DEBUG_ONLY_THIS_PROCESS,NULL,NULL,&stSi,&stPi);
	if(!bCreated){
		MessageBox(GetActiveWindow(),TEXT("不能打开test.exe文件！"),TEXT("Result"),MB_OK);
		//不能打开test.exe时清除资源
		if(szCmdLine!=NULL)  free(szCmdLine);
		return 0;
	}
	//开始调试运行，等待调试事件发生
	while(WaitForDebugEvent(&stDe,INFINITE)){//有调试事件发生
		switch(stDe.dwDebugEventCode)
		{
		case CREATE_PROCESS_DEBUG_EVENT:
			//写断点(int 3[0xCC])到nAddrOfBreakPoint，之前要保存原始指令
			ReadProcessMemory(stPi.hProcess,(LPVOID)nAddrOfBreakPoint,&cOldByte,1,NULL);
			WriteProcessMemory(stPi.hProcess,(LPVOID)nAddrOfBreakPoint,&cBreakPoint,1,NULL);
			break;
		case EXCEPTION_DEBUG_EVENT:
			if(stDe.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT)
			{//断点异常
				//若不是在nAddrOfBreakPoint处中断，则继续等待
				stThreadContext.ContextFlags = CONTEXT_CONTROL;
				GetThreadContext(stPi.hThread,&stThreadContext);
				if(stThreadContext.Eip != nAddrOfBreakPoint+1) 
					break;        
				//执行内存补丁
				WriteProcessMemory(stPi.hProcess,(LPVOID)0x004010B8,&cPatchByte,1,NULL);
				//恢复nAddrOfBreakPoint原始指令，并重新执行该指令
				WriteProcessMemory(stPi.hProcess,(LPVOID)nAddrOfBreakPoint,&cOldByte,1,NULL);
				stThreadContext.ContextFlags = CONTEXT_FULL;
				GetThreadContext(stPi.hThread,&stThreadContext);
				stThreadContext.Eip = nAddrOfBreakPoint;
				SetThreadContext(stPi.hThread,&stThreadContext);
			}
			break;
		case EXIT_PROCESS_DEBUG_EVENT:  
			bFinished = TRUE;
			
		}
		ContinueDebugEvent(stPi.dwProcessId,stPi.dwThreadId,DBG_CONTINUE);//继续让被调试的程序执行
		if(bFinished)break;
	}
	//破解成功后清除内存资源
	if(szCmdLine != NULL)  
		free(szCmdLine);
	CloseHandle(stPi.hThread);
	CloseHandle(stPi.hProcess);
	return 0;
}
