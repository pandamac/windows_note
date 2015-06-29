// MonitorTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
extern "C"
BOOL
WINAPI
ReadDirectoryChangesW(
  HANDLE hDirectory,                                  // handle to directory
  LPVOID lpBuffer,                                    // read results buffer
  DWORD nBufferLength,                                // length of buffer
  BOOL bWatchSubtree,                                 // monitoring option
  DWORD dwNotifyFilter,                               // filter conditions
  LPDWORD lpBytesReturned,                            // bytes returned
  LPOVERLAPPED lpOverlapped,                          // overlapped buffer
  LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine // completion routine
);

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
    BOOL bRet = FALSE;
    BYTE Buffer[1024] = { 0 };

    FILE_NOTIFY_INFORMATION *pBuffer = (FILE_NOTIFY_INFORMATION *)Buffer;
    DWORD BytesReturned = 0;
    HANDLE hFile = CreateFile("e:\\",
                FILE_LIST_DIRECTORY, 
                FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                FILE_FLAG_BACKUP_SEMANTICS,
                NULL);
    if ( INVALID_HANDLE_VALUE == hFile )
    {
        return 1;
    }

    printf("monitor... \r\n");

    while ( TRUE )
    {
        ZeroMemory(Buffer, 1024);
        bRet = ReadDirectoryChangesW(hFile,
                        &Buffer,
                        sizeof(Buffer),
                        TRUE,
                        FILE_NOTIFY_CHANGE_FILE_NAME |  // 修改文件名
                        FILE_NOTIFY_CHANGE_ATTRIBUTES | // 修改文件属性
                        FILE_NOTIFY_CHANGE_LAST_WRITE , // 最后一次写入
                        &BytesReturned,
                        NULL, NULL);
        
        if ( bRet == TRUE )
        {
            char szFileName[MAX_PATH] = { 0 };

            // 宽字符转换多字节
            WideCharToMultiByte(CP_ACP,
                                0,
                                pBuffer->FileName,
                                pBuffer->FileNameLength / 2,
                                szFileName,
                                MAX_PATH,
                                NULL,
                                NULL);

            switch(pBuffer->Action) 
            { 
                // 添加
            case FILE_ACTION_ADDED: 
                {
                    printf("添加 : %s\r\n", szFileName);
                    
                    break;
                }
                // 删除
            case FILE_ACTION_REMOVED: 
                {
                    printf("删除 : %s\r\n", szFileName);
                    
                    break; 
                }
                // 修改
            case FILE_ACTION_MODIFIED: 
                {
                    printf("修改 : %s\r\n", szFileName);
                    
                    break; 
                }
                // 重命名
            case FILE_ACTION_RENAMED_OLD_NAME: 
                {
                    printf("重命名 : %s", szFileName);
                    if ( pBuffer->NextEntryOffset != 0 )
                    {
                        FILE_NOTIFY_INFORMATION *tmpBuffer = (FILE_NOTIFY_INFORMATION *)((DWORD)pBuffer + pBuffer->NextEntryOffset);
                        switch ( tmpBuffer->Action )
                        {
                        case FILE_ACTION_RENAMED_NEW_NAME:
                            {        
                                ZeroMemory(szFileName, MAX_PATH);
                                WideCharToMultiByte(CP_ACP,
                                    0,
                                    tmpBuffer->FileName,
                                    tmpBuffer->FileNameLength / 2,
                                    szFileName,
                                    MAX_PATH,
                                    NULL,
                                    NULL);
                                printf(" ->  : %s \r\n", szFileName);
                                break;
                            }
                        }
                    }
                    break; 
                }
            case FILE_ACTION_RENAMED_NEW_NAME: 
                {
                    printf("重命名(new) : %s\r\n", szFileName);
                }
            }
        }
    }

    CloseHandle(hFile);

    return 0;
}

int main(int argc, char* argv[])
{
    HANDLE hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);
    if ( hThread == NULL )
    {
        return -1;
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

	return 0;
}
