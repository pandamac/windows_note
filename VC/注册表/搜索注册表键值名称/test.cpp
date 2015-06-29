#include <windows.h>
#include <stdio.h>

#define MAINKEY HKEY_LOCAL_MACHINE		// 主键
int ResultCount = 0;					// 符合条件的值数

// 下面这段代码是处理键值的，第一个参数是使用API从注册表中读出来的值
// 第二个参数是你要查找的值，比如病毒名，由于目的主要是演示，写的很简单
bool StealReg(char KeyValue[MAX_PATH],char Virus[MAX_PATH])
{	// 这里面要实现的具体功能大家可根据实际情况自行扩充
	if(strcmp(KeyValue,Virus) == 0)
	{
		return true;
	}
	return false;
}

// 该函数唯一的参数是SubKey，比如"software\\Microsoft"
void EnumReg(char SubKey[MAX_PATH])
{
	char temp[MAX_PATH];
	HKEY hKey = NULL;
	char str[MAX_PATH];	
	DWORD num = sizeof(str),index = 0,rc;
	
	rc = ::RegOpenKeyEx(MAINKEY,SubKey,0,KEY_ALL_ACCESS,&hKey);
	if(rc == ERROR_SUCCESS)
	{
		while( RegEnumValue(hKey,index,str,&num,NULL,NULL,NULL,NULL)==0 )
		{	//首先遍历值，进行处理
			printf("\t%s\n",str);
			if(StealReg(str,"F:\\BCB\\Program Files\\Common Files\\Borland Shared\\BDE\\"))
			{	//上面第二个参数就是你要查找的值 键值名称
				ResultCount++;////////////////
			}
			index++;
			num = MAX_PATH;
		}
		index = 0;
		while( RegEnumKeyEx(hKey,index,str,&num,NULL,NULL,NULL,NULL)==0 )
		{	//然后遍历子项后进行递归
			printf("%s\n",str);
			strcpy(temp,SubKey);
			strcat(temp,"\\");
			strcat(temp,str);
			EnumReg(temp);			//递归
			
			index++;
			num = MAX_PATH;
		}
	}
	else
	{
		printf("Can't Open The Key!\n");
	}
	
	RegCloseKey(hKey);
}

void main()
{
	EnumReg("software");
	printf("\n\n符合条件的值共有：%d 条!\n\n",ResultCount);
}