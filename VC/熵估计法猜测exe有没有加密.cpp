HANDLE hFile = CreateFileW(FileName.toStdWString().c_str(),GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,NULL,NULL);
	if(hFile == INVALID_HANDLE_VALUE) return false;
		
	HANDLE hFileMap = CreateFileMapping(hFile,NULL,PAGE_READONLY,NULL,NULL,NULL);

	m_fileBuffer = MapViewOfFile(hFileMap,FILE_MAP_READ,NULL,NULL,NULL);
	if(m_fileBuffer == NULL)
	{
		CloseHandle(hFile);
		CloseHandle(hFileMap);

		return false;
	}

float loadEntropie(DWORD fileSize)//熵估计，各种方法，基于有限的样本信息熵的统计估计
{
	DWORD64 countBytes[256] = { 0 };//the numbers of calls
	long double result = 0.0, temp = 0.0;

	for(DWORD i = 0; i < fileSize; i++)
	{
		countBytes[((PBYTE)m_fileBuffer)[i]]++;//m_fileBuffer
	}

	for(int i = 0; i < 256; i++)
	{
		if(countBytes[i] == 0) continue;

		temp = (long double)countBytes[i] / (long double)fileSize;
		result -= temp * log(temp);//返回某个数字的自然对数（底为 e）。
	}

	result /= log((long double)256);
	result *= 8;

	return result;
}

float fileEntropie = getEntropie(filepath);
		if(fileEntropie >= 6.4) // TODO: more research for the value
		{
			"Nanomite", "It seems that this file is packed or encrypted!\nThis can lead to wrong display (or none) of disassembly!"
		}