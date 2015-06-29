#include <windows.h>
#include "stdio.h"

#define ThreadCound 4 //创建线程个数
DWORD dwTlsIndex;

//全局变量
int iNUM_OF_CALL_COMMON=0;   
int iNUM_OF_CALL_THREAD=0;


VOID CommonFunc(VOID) 
{ 
        LPVOID lpvData; 
        // Retrieve a data pointer for the current thread. 
        iNUM_OF_CALL_COMMON++;
        
        lpvData = TlsGetValue(dwTlsIndex); 
        if ((lpvData == 0) && (GetLastError() != ERROR_SUCCESS)) 
			exit(0);
        
        // Use the data stored for the current thread. 
		printf("common: thread %d: lpvData=%lx\n", 
			GetCurrentThreadId(), lpvData); 
        
        Sleep(5000); 
} 


DWORD WINAPI ThreadFunc(VOID) 
{ 
	LPVOID lpvData; 
	
	// Initialize the TLS index for this thread. 
	iNUM_OF_CALL_THREAD++;
	
	lpvData = (LPVOID) LocalAlloc(LPTR, 256); 
	if (! TlsSetValue(dwTlsIndex, lpvData)) 
		exit(0); 
	
	printf("thread %d: lpvData=%lx\n", GetCurrentThreadId(), lpvData); 
	
	CommonFunc(); 
	
	// Release the dynamic memory before the thread returns. 
    lpvData = TlsGetValue(dwTlsIndex); 
	if (lpvData != 0) 
		LocalFree((HLOCAL) lpvData); 
	
	return 0; 
} 


void main(void)
{
	DWORD dwThreadId;
	HANDLE hThread[ThreadCound];
	int i;
	//Allocate a TLS index
	if ((dwTlsIndex =TlsAlloc()) == TLS_OUT_OF_INDEXES)
	{
		exit(0);
	}
	
	for (i = 0; i<ThreadCound; i++)
	{
		hThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, NULL, 0, &dwThreadId);
		if(hThread == NULL)
		{
			exit(0);
		}
	}
	for (i = 0; i < ThreadCound; i++)
	{
		WaitForSingleObject(hThread, INFINITE);
	}
	TlsFree(dwTlsIndex);
	
	printf("The nums of thread is: %d\n",iNUM_OF_CALL_THREAD);
	printf("The nums of call is: %d\n",iNUM_OF_CALL_COMMON);

}
