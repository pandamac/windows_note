// ParseMbr.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>


#define BOOTRECORDSIZE 440

typedef struct _BOOTRECORD
{
    unsigned char BootRecord[BOOTRECORDSIZE];
}BOOTRECORD, *PBOOTRECORD;

#define DPTSIZE 64

typedef struct _DPT
{
    unsigned char Dpt[DPTSIZE];
}DPT, *PDPT;

#define DPTNUMBER 4

typedef struct _DP
{
    unsigned char BootSign;         // 引导标志
    unsigned char StartHsc[3];
    unsigned char PartitionType;    // 分区类型
    unsigned char EndHsc[3];
    ULONG         SectorsPreceding;     // 本分区之前使用的扇区数
    ULONG         SectorsInPartition;   // 分区的总扇区数
}DP, *PDP;

typedef struct _MBR
{
    BOOTRECORD BootRecord;                  // 引导程序
    unsigned char ulSigned[4];              // Windows磁盘签名
    unsigned char sReserve[2];              // 保留位
    DPT        Dpt;                         // 分区表
    unsigned char EndSign[2];               // 结束标志
}MBR, *PMBR;

// 显示MBR数据
VOID ShowMbr(HANDLE hDevice, PMBR pMbr)
{    
    DWORD dwRead = 0;
    ReadFile(hDevice, (LPVOID)pMbr, sizeof(MBR), &dwRead, NULL);
    
    for ( int i = 0; i < 512; i ++ )
    {
        printf("%02X ", ((BYTE *)pMbr)[i]);
        if ( ( i + 1 ) % 16 == 0 )
        {
            printf("\r\n");
        }
    }
}

// 解析MBR
VOID ParseMbr(MBR Mbr)
{
    printf("引导记录: \r\n");

    for ( int i = 0; i < BOOTRECORDSIZE; i ++ )
    {
        printf("%02X ", Mbr.BootRecord.BootRecord[i]);
        if ( ( i + 1 ) % 16 == 0 )
        {
            printf("\r\n");
        }
    }

    printf("\r\n");

    printf("磁盘签名: \r\n");
    for ( i = 0; i < 4; i ++ )
    {
        printf("%02X ", Mbr.ulSigned[i]);
    }

    printf("\r\n");
    
    printf("解析分区表: \r\n");
    for ( i = 0; i < DPTSIZE; i ++ )
    {
        printf("%02X ", Mbr.Dpt.Dpt[i]);
        if ( ( i + 1 ) % 16 == 0 )
        {
            printf("\r\n");
        }
    }

    printf("\r\n");

    PDP pDp = (PDP)&(Mbr.Dpt.Dpt);
    for ( i = 0; i < DPTNUMBER; i ++ )
    {
        printf("引导标志: %02X ", pDp[i].BootSign);
        printf("分区类型: %02X", pDp[i].PartitionType);
        printf("\r\n");
        printf("本分区之前扇区数: %d ", pDp[i].SectorsPreceding);
        printf("本分区的总扇区数: %d", pDp[i].SectorsInPartition);
        printf("\r\n");        
        printf("该分区的大小: %f \r\n", (double)pDp[i].SectorsInPartition / 1024 * 512 / 1024 / 1024 );

        printf("\r\n \r\n");
    }


    printf("结束标志: \r\n");
    for ( i = 0; i < 2; i ++ )
    {
        printf("%02X ", Mbr.EndSign[i]);
    }

    printf("\r\n");
}

int main(int argc, char* argv[])
{
    // 打开物理硬盘设备
    HANDLE hDevice = CreateFile("\\\\.\\PhysicalDrive0",
                    GENERIC_READ,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL,
                    OPEN_EXISTING,
                    0,
                    NULL);
    if ( hDevice == INVALID_HANDLE_VALUE )
    {
        printf("CreateFile Error %d \r\n", GetLastError());
        return -1;
    }

    MBR Mbr = { 0 };
    ShowMbr(hDevice, &Mbr);
    ParseMbr(Mbr);

    CloseHandle(hDevice);

	return 0;
}
