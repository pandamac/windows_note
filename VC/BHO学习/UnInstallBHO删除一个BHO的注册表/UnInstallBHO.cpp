// UnInstallBHO.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <tchar.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")


bool UnInstallBHO(TCHAR *pCLSID, bool bDeleteFile = FALSE)
{
	HKEY 	hBhoMainKey = HKEY_LOCAL_MACHINE;
	HKEY 	hClsidMainKey = HKEY_CLASSES_ROOT;
	HKEY 	hBhoSubKey;
	HKEY 	hClsidSubKey;
	TCHAR   tcBuffer[256] = {0};
	TCHAR   tcSrcCLSID[24];
	_tcscpy(tcSrcCLSID, pCLSID);
	TCHAR   *pSrcCLSID = _tcsupr(tcSrcCLSID);
	TCHAR   *pDstCLSID = NULL;
	BOOL    bRet = FALSE;
	//打开HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects键句柄，并获得枚举权限，以枚举其下面的子键
	LONG ReturnValue = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects",
		0L, KEY_ENUMERATE_SUB_KEYS, &hBhoSubKey);
	int i = 0, retCode = 0;
	DWORD dwMaxValueLen = 0, dwValueSize = 0, dwValueType = 0;
	if (ReturnValue == ERROR_SUCCESS) //如果成功打开
	{   
		TCHAR  tcKeyName[128];    //用于保存每个子键的名字的内存空间
		DWORD dwKeyNameSize = 128; 
		while (1)
		{   //获取每个子键的名称
			dwKeyNameSize = 128;
			retCode = RegEnumKeyEx(hBhoSubKey, i, tcKeyName, &dwKeyNameSize, 
				NULL, NULL, NULL, NULL); 
			if (retCode == ERROR_NO_MORE_ITEMS)
			{   //如果获取结束则跳出循环
				break;
			}
			pDstCLSID = _tcsupr(tcKeyName);
			if (_tcscmp(pSrcCLSID, pDstCLSID) != 0)
			{	//没找到要删除的BHO的CLSID，继续查找下一个
				i++;
				continue;
			}
			_stprintf(tcBuffer, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects\\%s"), tcKeyName,
				pDstCLSID);
			ReturnValue = SHDeleteKey(hBhoMainKey, tcBuffer);
			if (ReturnValue != ERROR_SUCCESS)
			{
				break;
			}
			_stprintf(tcBuffer, _T("CLSID\\%s\\InprocServer32"), pDstCLSID);
			if (bDeleteFile)
			{
				ReturnValue = RegOpenKeyEx (hClsidMainKey, tcBuffer,
					0L, KEY_ENUMERATE_SUB_KEYS|KEY_QUERY_VALUE, &hClsidSubKey);
				if (ReturnValue != ERROR_SUCCESS)
				{
					break;
				}
				//查询该子键下的值数据所需的最大内存空间
				ReturnValue = RegQueryInfoKey(hClsidSubKey, NULL, NULL, NULL, NULL, 
					NULL, NULL, NULL, NULL, &dwMaxValueLen, NULL, NULL);
				if (ReturnValue != ERROR_SUCCESS)
				{
					RegCloseKey(hClsidSubKey);
					break;
				}
				PBYTE pValueData = new BYTE[dwMaxValueLen];
				dwValueSize = dwMaxValueLen;
				ReturnValue = RegQueryValueEx(hClsidSubKey, NULL, NULL, 
					&dwValueType, pValueData, &dwValueSize); 
				if (ReturnValue != ERROR_SUCCESS)
				{
				}
				if (!DeleteFile((CHAR *)pValueData))
				{
					RegCloseKey(hClsidSubKey);
					delete []pValueData;
					break;
				}
				delete []pValueData;
			}
			ReturnValue = SHDeleteKey(hClsidMainKey, tcBuffer);
			if (ReturnValue == ERROR_SUCCESS)
			{
				bRet = TRUE;
			}
			break;
		}
		RegCloseKey(hBhoSubKey); //关闭新建子键的句柄释放资源。	
	}
	return bRet;
}


int main(int argc, char* argv[])
{
	UnInstallBHO("{3FE8E6B8-6CDF-44B0-B250-0836B2320B7E}", true);
	return 0;
}

