void CDemoDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString s = TEXT("*********** C ***********\r\n");
	char line[256];
	FILE *pFile = fopen("book.txt","r");
	while(fgets(line, 256, pFile) != NULL)
	{
		s += line;
		s += TEXT("\r\n");
	}
	fclose(pFile);
	SetDlgItemText(IDC_EDIT1,s);
}
///////////////////////////////////////////////////////////////
	FILE *inFile, *outFile;
	char rec[BUF_SIZE];
	size_t bytesIn, bytesOut;

	if(argc != 3)
	{
		printf("Usage: cpc file1 file2\n");
		return 1;
	}

	inFile = fopen(argv[1], "rb");
	if(inFile == NULL)
	{
		perror(argv[1]);
		return 2;
	}

	outFile = fopen(argv[2], "wb");
	if(outFile == NULL)
	{
		perror(argv[2]);
		return 3;
	}

	while((bytesIn = fread(rec,1,BUF_SIZE,inFile))>0)
	{
		bytesOut = fwrite(rec,1,bytesIn,outFile);
		if(bytesIn != bytesOut)
		{
			perror("Fatal write error.");
			return 4;
		}
	}

	fclose(inFile);
	fclose(outFile);
///////////////////////////////////////////////////////////////
void CDemoDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString s = TEXT("*********** C++ ***********\r\n");
	char line[256];
	std::ifstream ifs("book.txt");
	while(ifs.getline(line,256))
	{
		s += line;
		s += TEXT("\r\n");
	}
	ifs.close();
	SetDlgItemText(IDC_EDIT1,s);
}
///////////////////////////////////////////////////////////////
void CDemoDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	CString s = TEXT("*********** API ***********\r\n");
	CHAR lpFileDataBuffer[4096];
	HANDLE hFile;
	DWORD dwReadedSize;
	ZeroMemory(lpFileDataBuffer,4096);

	hFile = CreateFile(
		TEXT("book.txt"),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	ReadFile(hFile, lpFileDataBuffer, 4096, &dwReadedSize,NULL);
	CloseHandle(hFile);
	s += lpFileDataBuffer;
	SetDlgItemText(IDC_EDIT1, s);
}
///////////////////////////////////////////////////////////////
	HANDLE hIn, hOut;
	DWORD nIn, nOut;
	CHAR buffer[BUF_SIZE];

	if(argc != 3)
	{
		printf("Usage: cpw file1 file2\n");
		return 1;
	}

	hIn = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hIn == INVALID_HANDLE_VALUE)
	{
		printf("Cannot open input file. Error: %x\n", GetLastError());
		return 2;
	}

	hOut = CreateFile(argv[2], GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hOut == INVALID_HANDLE_VALUE)
	{
		printf("Cannot open output file. Error: %x\n", GetLastError());
		return 3;
	}

	while(ReadFile(hIn, buffer, BUF_SIZE, &nIn, NULL) && nIn > 0)
	{
		WriteFile(hOut, buffer, nIn, &nOut, NULL);
		if(nIn != nOut)
		{
			printf("Fatal write error: %x\n", GetLastError());
			return 4;
		}
	}
	CloseHandle(hIn);
	CloseHandle(hOut);
///////////////////////////////////////////////////////////////
	if(argc != 3)
	{
		printf("Usage: cpcf file1 file2\n");
		return 1;
	}
	if(!CopyFile(argv[1], argv[2], FALSE))
	{
		printf("CopyFile Error: %x\n", GetLastError());
		return 2;
	}
///////////////////////////////////////////////////////////////
void CDemoDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	CString s = TEXT("*********** MFC ***********\r\n");
	CString line;
	CStdioFile file(TEXT("book.txt"), CFile::modeRead);
	while(file.ReadString(line))
	{
		s += line;
		s += TEXT("\r\n");
	}
	SetDlgItemText(IDC_EDIT1,s);
}

void CDemoDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	SetDlgItemText(IDC_EDIT1, TEXT(""));
}