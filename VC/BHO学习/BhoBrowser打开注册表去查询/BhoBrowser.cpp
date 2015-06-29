// BhoBrowser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

int main(int argc, char* argv[])
{
	HKEY 	hBhoMainKey = HKEY_LOCAL_MACHINE;
	HKEY 	hClsidMainKey = HKEY_CLASSES_ROOT;
	HKEY 	hBhoSubKey;
	HKEY 	hClsidSubKey;
	TCHAR   tcBuffer[256] = {0};
	DWORD   dwMaxValueLen = 0, dwValueType = 0, dwValueSize = 0;
	//打开HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects键句柄，并获得枚举权限，以枚举其下面的子键
	LONG ReturnValue = RegOpenKeyEx (HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects",
		0L, KEY_ENUMERATE_SUB_KEYS, &hBhoSubKey);
	int i = 0, retCode = 0;
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
			printf("%s\n", tcKeyName);
			//打开HKEY_CLASSES_ROOT\CLSID\xxxxx\InprocServer32键句柄，并获得查询权限，以查询其默认子键值。
			_stprintf(tcBuffer, _T("CLSID\\%s\\InprocServer32"), tcKeyName);
			ReturnValue = RegOpenKeyEx (hClsidMainKey, tcBuffer,
				0L, KEY_ENUMERATE_SUB_KEYS|KEY_QUERY_VALUE, &hClsidSubKey);
			if (ReturnValue != ERROR_SUCCESS)
			{
				i++;
				continue;
			}
			//查询该子键下的值数据所需的最大内存空间
			ReturnValue = RegQueryInfoKey(hClsidSubKey, NULL, NULL, NULL, NULL, 
				NULL, NULL, NULL, NULL, &dwMaxValueLen, NULL, NULL);
			if (ReturnValue != ERROR_SUCCESS)
			{
				RegCloseKey(hClsidSubKey);
				i++;
				continue;
			}
			PBYTE pValueData = new BYTE[dwMaxValueLen];
			dwValueSize = dwMaxValueLen;
			ReturnValue = RegQueryValueEx(hClsidSubKey, NULL, NULL, 
				&dwValueType, pValueData, &dwValueSize); 
			if (ReturnValue != ERROR_SUCCESS)
			{
				RegCloseKey(hClsidSubKey);
				i++;
				delete []pValueData;
				continue;
			}
			printf("%s\n\n", pValueData);
			delete []pValueData;
			RegCloseKey(hClsidSubKey);
			i++;
		}
		RegCloseKey(hBhoSubKey);
	}
	return 0;
}

