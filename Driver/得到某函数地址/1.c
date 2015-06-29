/**************************************************************************************************/
//得到DLL中函数的地址,读取文件解析PE结构来实现
DWORD GetDllFunctionAddress(char* lpFunctionName,PUNICODE_STRING pDllName)
{
	HANDLE hThread, hSection, hFile, hMod;
	SECTION_IMAGE_INFORMATION sii;
	IMAGE_DOS_HEADER* dosheader;
	IMAGE_OPTIONAL_HEADER* opthdr;
	IMAGE_EXPORT_DIRECTORY* pExportTable;
	DWORD* arrayOfFunctionAddresses;
	DWORD* arrayOfFunctionNames;
	WORD* arrayOfFunctionOrdinals;
    //DWORD functionOrdinal;
	WORD functionOrdinal;
	DWORD Base, x, functionAddress;
	char* functionName;
	STRING ntFunctionName, ntFunctionNameSearch;
	PVOID BaseAddress = NULL;
	SIZE_T size=0;
	OBJECT_ATTRIBUTES oa = {sizeof oa, 0, pDllName, OBJ_CASE_INSENSITIVE};
	IO_STATUS_BLOCK iosb;
	
	ZwOpenFile(&hFile, FILE_EXECUTE | SYNCHRONIZE, &oa, &iosb, FILE_SHARE_READ, FILE_SYNCHRONOUS_IO_NONALERT);
	oa.ObjectName = 0;

	ZwCreateSection(&hSection, SECTION_ALL_ACCESS, &oa, 0,PAGE_EXECUTE, SEC_IMAGE, hFile);

	ZwMapViewOfSection(hSection, NtCurrentProcess(), &BaseAddress, 0, 1000, 0, &size, (SECTION_INHERIT)1, MEM_TOP_DOWN, PAGE_READWRITE);

	ZwClose(hFile);
	/*

	IMAGE_NT_HEADERS STRUCT
        Signature DWORD ? ；PE 文件标识
        FileHeader IMAGE_FILE_HEADER <>
        OptionalHeader IMAGE_OPTIONAL_HEADER32 <>
    IMAGE_NT_HEADERS ENDS


	IMAGE_OPTIONAL_HEADER32 STRUCT
		Magic WORD ? ;0018h 107h＝ROM Image,10Bh=exe Image
		MajorLinkerVersion BYTE ? ;001ah 链接器版本号
		MinorLinkerVersion BYTE ? ;001bh
		SizeOfCode DWORD ? ;001ch 所有含代码的节的总大小
		SizeOfInitializedData DWORD? ;0020h 所有含已初始化数据的节的总大小
		SizeOfUninitializedData DWORD ? ;0024h 所有含未初始化数据的节的大小
		AddressOfEntryPoint DWORD ? ;0028h 程序执行入口RVA
		BaseOfCode DWORD ? ;002ch 代码的节的起始RVA
		BaseOfData DWORD ? ;0030h 数据的节的起始RVA
		ImageBase DWORD ? ;0034h 程序的建议装载地址
		SectionAlignment DWORD ? ;0038h 内存中的节的对齐粒度
		FileAlignment DWORD ? ;003ch 文件中的节的对齐粒度
		MajorOperatingSystemVersion WORD ? ;0040h 操作系统主版本号
		MinorOperatingSystemVersion WORD ? ;0042h 操作系统副版本号
		MajorImageVersion WORD ? ;0044h 可运行于操作系统的最小版本号
		MinorImageVersion WORD ? ;0046h
		MajorSubsystemVersion WORD ?;0048h 可运行于操作系统的最小子版本号
		MinorSubsystemVersion WORD ? ;004ah
		Win32VersionValue DWORD ? ;004ch 未用
		SizeOfImage DWORD ? ;0050h 内存中整个PE 映像尺寸
		SizeOfHeaders DWORD ? ;0054h 所有头＋节表的大小
		CheckSum DWORD ? ;0058h
		Subsystem WORD ? ;005ch 文件的子系统
		DllCharacteristics WORD ? ;005eh
		SizeOfStackReserve DWORD ? ;0060h 初始化时的堆栈大小
		SizeOfStackCommit DWORD ? ;0064h 初始化时实际提交的堆栈大小
		SizeOfHeapReserve DWORD ? ;0068h 初始化时保留的堆大小
		SizeOfHeapCommit DWORD ? ;006ch 初始化时实际提交的堆大小
		LoaderFlags DWORD ? ;0070h 未用
		NumberOfRvaAndSizes DWORD ? ;0074h 下面的数据目录结构的数量
		DataDirectory IMAGE_DATA_DIRECTORY 16 dup(<>) ;0078h
	IMAGE_OPTIONAL_HEADER32 ENDS

	DataDirectory 字段

	这个字段可以说是最重要的字段之一，它由16 个相同的IMAGE_DATA_DIRECTORY 结构组成，虽然PE 文件中
	的数据是按照装入内存后的页属性归类而被放在不同的节中的，但是这些处于各个节中的数据按照用途可
	以被分为导出表、导入表、资源、重定位表等数据块，这16 个IMAGE_DATA_DIRECTORY 结构就是用来定义
	多种不同用途的数据块的。IMAGE_DATA_DIRECTORY 结构的定义很简单，它仅仅指出了某
	种数据块的位置和长度。
	IMAGE_DATA_DIRECTORY STRUCT
	   VirtualAddress DWORD ? ；数据的起始RVA
	   Size DWORD ? ；数据块的长度
	IMAGE_DATA_DIRECTORY ENDS

	一共有十六个IMAGE_DATA_DIRECTORYENDS 结构
	IMAGE_DIRECTORY_ENTRY_EXPORT 导出表
	IMAGE_DIRECTORY_ENTRY_IMPORT 导入表
	IMAGE_DIRECTORY_ENTRY_RESOURCE 资源
	IMAGE_DIRECTORY_ENTRY_EXCEPTION 异常（具体资料不详）
	IMAGE_DIRECTORY_ENTRY_SECURITY 安全（具体资料不详）
	IMAGE_DIRECTORY_ENTRY_BASERELOC 重定位表
	IMAGE_DIRECTORY_ENTRY_DEBUG 调试信息
	IMAGE_DIRECTORY_ENTRY_ARCHITECTURE 版权信息
	IMAGE_DIRECTORY_ENTRY_GLOBALPTR 具体资料不详
	IMAGE_DIRECTORY_ENTRY_TLS Thread Local Storage
	IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG 具体资料不详
	IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT 具体资料不详
	IMAGE_DIRECTORY_ENTRY_IAT 导入函数地址表
	IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT 具体资料不详
	IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 具体资料不详
	未使用保留

	Public Type IMAGE_EXPORT_DIRECTORY
		Characteristics As Long
		TimeDateStamp As Long
		MajorVersion As Integer
		MinorVersion As Integer
		Name1 As Long
		Base As Long
		NumberOfFunctions As Long
		NumberOfNames As Long
		AddressOfFunctions As Long
		AddressOfNames As Long
		AddressOfNameOrdinals As Long
    End Type

	*/
	hMod = BaseAddress;//基地址
	dosheader = (IMAGE_DOS_HEADER *)hMod;//DOS文件头
	opthdr =(IMAGE_OPTIONAL_HEADER *) ((BYTE*)hMod+dosheader->e_lfanew+24);//hMod+dosheader->e_lfanew指向PE文件头    +24指向 IMAGE_OPTIONAL_HEADER
	pExportTable =(IMAGE_EXPORT_DIRECTORY*)((BYTE*) hMod + opthdr->DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT]. VirtualAddress);
	
	arrayOfFunctionAddresses = (DWORD*)( (BYTE*)hMod + pExportTable->AddressOfFunctions);
	arrayOfFunctionNames = (DWORD*)( (BYTE*)hMod + pExportTable->AddressOfNames);
	arrayOfFunctionOrdinals = (WORD*)( (BYTE*)hMod + pExportTable->AddressOfNameOrdinals);
	Base = pExportTable->Base;
	RtlInitString(&ntFunctionNameSearch, lpFunctionName);
	for(x = 0; x < pExportTable->NumberOfFunctions; x++)
	{
		functionName = (char*)( (BYTE*)hMod + arrayOfFunctionNames[x]);
		RtlInitString(&ntFunctionName, functionName);
		functionOrdinal = (WORD)(arrayOfFunctionOrdinals[x] + Base - 1); 
		functionAddress = (DWORD)( (BYTE*)hMod + arrayOfFunctionAddresses[functionOrdinal]);
		if (RtlCompareString(&ntFunctionName, &ntFunctionNameSearch, TRUE) == 0)
		{
			ZwClose(hSection);
			return functionAddress;
		}
	}
	ZwClose(hSection);
	return 0;
}
//得到函数地址
void GetSpecialFunctionAddr()
{
	UNICODE_STRING DllName;
	RtlInitUnicodeString(&DllName,L"\\Device\\HarddiskVolume1\\Windows\\System32\\kernel32.dll");

	g_pBufferForFunc = (PVOID)ExAllocatePoolWithTag(0, 0xDCu,'kdD');
	pPoolAddr = (int)g_pBufferForFunc;
	if ( pPoolAddr ) 
	{
		*(DWORD *)(pPoolAddr) = GetDllFunctionAddress( "LoadLibraryA", &DllName);
		DbgPrint("LoadLibraryA== %08X\n", *(DWORD *)(pPoolAddr));
	}
}