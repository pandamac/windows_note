void EnumProcessByPspCidTable()
{
    ULONG pCidTableAddr = 0;
    PHANDLE_TABLE pCidHandleTable = NULL;
    PHANDLE_TABLE_ENTRY pTable1, *pTable2, **pTable3;
    ULONG pRealHandleTable = 0;
    ULONG level = 0;
    ULONG uMax_Handle = 0;

    //得到PspCidTable地址
    pCidTableAddr = GetCidTableAddr();
    pCidHandleTable = (PHANDLE_TABLE)(*(PULONG)pCidTableAddr);

    // 得到表的级数（HANDLE_TALBE中TableCode的低两位即使表的级数）
    // 所以和3 与运算即可（取低两位）
    level = (pCidHandleTable->TableCode) & 0x3;

    // 得到真正的TableCode地址，指向HANDLE_TALBE_ENTRY结构
    pRealHandleTable = (pCidHandleTable->TableCode) & ~0x3;

    // 得到句柄值的最大数。
    uMax_Handle = pCidHandleTable->NextHandleNeedingPool;

    switch( level )
    {
    case 0:
        {
            ULONG index = 0;
            pTable1 = (PHANDLE_TABLE_ENTRY)(pRealHandleTable);
            for( index = 0; index < MAX_ENT_CNT; index++ )
            {
                if( pTable1[index].Object != NULL )
                {
                    ULONG pObject = (ULONG)(pTable1[index].Object) & ~7 ;
                    if( MmIsAddressValid((PULONG)(pObject - 0x10)) )
                    {
                        POBJECT_TYPE pType = (POBJECT_TYPE)(*(PULONG)(pObject - 0x10));
                        if( pType == *PsProcessType )
                        {
                            DbgPrint("PId:%d\tPath:%s\n", index*4, PsGetProcessImageFileName((PEPROCESS)pObject) );
                        }
                    }

                }
            }
            break;
        }
    case 1:
        {
            ULONG index = 0;
            pTable2 = (PHANDLE_TABLE_ENTRY*)(pRealHandleTable);
            for( index = 0; index < uMax_Handle/(4*MAX_ENT_CNT); index++ )
            {
                pTable1 = pTable2[index];
                if( pTable1 == NULL )
                    break;
                else
                {
                    ULONG i = 0;
                    for( i = 0; i < MAX_ENT_CNT; i++ )
                    {
                        if( pTable1[i].Object != NULL )
                        {
                            ULONG pObject = (ULONG)(pTable1[i].Object) & ~7;
                            if( MmIsAddressValid( (PULONG)(pObject-0x10) ) )
                            {
                                POBJECT_TYPE pType = (POBJECT_TYPE)(*(PULONG)(pObject-0x10));
                                if( pType == *PsProcessType )
                                {
                                    DbgPrint("PId:%d\tPath:%s\n", index*MAX_ENT_CNT*4+i*4, PsGetProcessImageFileName((PEPROCESS)pObject) );
                                }
                            }

                        }
                    }
                }
            }
            break;
        }
    case 2:
        {
            ULONG index = 0;
            pTable3 = (PHANDLE_TABLE_ENTRY**)(pRealHandleTable);
            for( index = 0; index < uMax_Handle/(MAX_ADD_CNT*MAX_ENT_CNT*4); index++ )
            {
                ULONG i = 0;
                pTable2 = (PHANDLE_TABLE_ENTRY*)((ULONG)pTable3[index] & ~0x3);
                if( pTable2 == NULL )
                    break;
                for( i = 0; i < MAX_ADD_CNT; i++ )
                {

                    pTable1 = pTable2[i];
                    if( pTable1 == NULL )
                        break;
                    else
                    {
                        ULONG j = 0;
                        for( j = 0; j < MAX_ENT_CNT; j++ )
                        {
                            if( pTable1[j].Object != NULL )
                            {
                                ULONG pObject = (ULONG)(pTable1[j].Object) & ~7;
                                if( MmIsAddressValid( (PULONG)(pObject-0x10) ) )
                                {
                                    POBJECT_TYPE pType = (POBJECT_TYPE)(*(PULONG)(pObject-0x10));
                                    if( pType == *PsProcessType )
                                    {
                                        DbgPrint("PId:%d\tPath:%s\n", index*MAX_ADD_CNT*MAX_ENT_CNT*4+i*MAX_ENT_CNT*4+j*4,\
                                            PsGetProcessImageFileName((PEPROCESS)pObject) );
                                    }
                                }
                            }


                        }

                    }
                }

            }

        }
        break;
    }


}

// 利用KPCR获取pspCidTable地址
// 测试后发现和在windbg中使用dd PspCidTable一样。
ULONG GetCidTableAddr()
{
    ULONG pCidHandleAddr = 0;
    _asm
    {
        mov eax,fs:[0x34]    //kpcr中的 KdVersionBlock
        mov eax,[eax+0x80]   // KdVersionBlock偏移0x80就是pspCidTable地址（观察总结的吧～）
        mov pCidHandleAddr,eax
    }
    return pCidHandleAddr;
}
