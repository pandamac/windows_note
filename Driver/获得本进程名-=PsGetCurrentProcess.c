
extern"C" __declspec(dllimport)UCHAR*PsGetProcessImageFileName(IN PEPROCESS Process);

#pragma PAGEDCODE
VOID GetPath()
{
	//**********************************************************************************************************************//
	PEPROCESS process;
	CHAR *pName;   //进程名
	process =PsGetCurrentProcess();
	pName = (CHAR*)PsGetProcessImageFileName(process); //获取进程名
	KdPrint(("%s\n",pName));
	//**********************************************************************************************************************//
}