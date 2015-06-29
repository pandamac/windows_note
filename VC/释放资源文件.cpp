/* 
函数功能：释放资源文件 
 
参数说明：DWORD dwResName   指定要释放的资源ID号，如IDR_EXE 
      LPCSTR lpResType 指定释放的资源的资源类型，如"EXE" 
      LPCSTR lpFilePathName 指定释放后的目标文件名,如"D:\\Test.exe" 
 
返回值：成功则返回TRUE,失败返回FALSE 
*/  
BOOL FreeResFile(DWORD dwResName, LPCSTR lpResType, LPCSTR lpFilePathName )  
{     
    HMODULE hInstance = ::GetModuleHandle(NULL);//得到自身实例句柄  
      
    HRSRC hResID = ::FindResource(hInstance,MAKEINTRESOURCE(dwResName),lpResType);//查找资源  
    HGLOBAL hRes = ::LoadResource(hInstance,hResID);//加载资源  
    LPVOID pRes = ::LockResource(hRes);//锁定资源  
      
    if (pRes == NULL)//锁定失败  
    {  
        return FALSE;  
    }  
    DWORD dwResSize = ::SizeofResource(hInstance,hResID);//得到待释放资源文件大小  
    HANDLE hResFile = CreateFile(lpFilePathName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);//创建文件  
      
    if (INVALID_HANDLE_VALUE == hResFile)  
    {  
        //TRACE("创建文件失败！");  
        return FALSE;  
    }  
  
    DWORD dwWritten = 0;//写入文件的大小     
    WriteFile(hResFile,pRes,dwResSize,&dwWritten,NULL);//写入文件  
    CloseHandle(hResFile);//关闭文件句柄  
      
    return (dwResSize == dwWritten);//若写入大小等于文件大小，返回成功，否则失败  
}  