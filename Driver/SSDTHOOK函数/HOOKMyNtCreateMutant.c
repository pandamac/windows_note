#include <ntddk.h>

typedef struct _SystemServiceDescriptorTable 
{ 
    PVOID    ServiceTableBase; //SSDT表的基地址
    PULONG    ServiceCounterTableBase; //指向另一个索引表，该表包含了每个服务表项被调用的次数
    ULONG    NumberOfService; //当前系统所支持的服务个数
    ULONG    ParamTableBase; //包含了每个服务所需的参数字节数
}SystemServiceDescriptorTable,*PSystemServiceDescriptorTable; 
extern PSystemServiceDescriptorTable KeServiceDescriptorTable; //导出函数，DDK的头文件中并未声明


typedef NTSTATUS(*NTCREATEMUTANT)(
                                  IN PUNICODE_STRING DriverServiceName
                                  );

NTCREATEMUTANT pRealNtCreateMutantAddr;

ULONG RealServiceAddress;  //接受被hook的函数地址


NTSTATUS MyNtCreateMutant(
                          IN PUNICODE_STRING DriverServiceName
                          )
                          
//我们自己的NtCreateMutant
{

 DriverServiceName=DriverServiceName+1;
    // MutantHandle=MutantHandle+1;//伪造一个错误的句柄
     //return STATUS_ACCESS_DENIED;//给他直接返回
  

  return (NTSTATUS)(NTCREATEMUTANT)pRealNtCreateMutantAddr( DriverServiceName);
      
}

NTSTATUS Hook()
{
  ULONG Address;

  Address = (ULONG)KeServiceDescriptorTable->ServiceTableBase + 97* 4;//43可以用xt工具查看
  RealServiceAddress = *(ULONG*)Address;  
  pRealNtCreateMutantAddr = (NTCREATEMUTANT)RealServiceAddress;

  //开启SSDT 表为可写
  _asm
  {
    cli;
    mov  eax, cr0;
    and eax, not 10000h;
    mov  cr0, eax;
  }

  *((ULONG*)Address) = (ULONG)MyNtCreateMutant;  //替换为我们自己的MyNtCreateMutant函数
  //设置SSDT表位只读
  _asm
  {
    mov  eax, cr0;
    or  eax, 10000h;
    mov  cr0, eax;
    sti;  
  }
  
}
VOID UnHook()//把SSDT中的NtCreateMutant函数还原
{
  ULONG Address;
  
  Address = (ULONG)KeServiceDescriptorTable->ServiceTableBase + 97 * 4;

  
  __asm 
     { 
       cli 
       mov    eax, cr0;
       and    eax, not 10000h ;
       mov    cr0, eax ;
     } 
  
     *((ULONG*)Address) = (ULONG)RealServiceAddress; 
    
  __asm 
     { 
       mov    eax, cr0 ;
       or   eax, 10000h ;
       mov    cr0, eax ;
       sti ;
     } 
}



VOID Unload(PDRIVER_OBJECT driver)
{
  UNREFERENCED_PARAMETER(driver);
  UnHook();
  DbgPrint(("驱动退出"));
  
}
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
  NTSTATUS status;
  UNREFERENCED_PARAMETER(str);
  driver->DriverUnload = Unload;

  status = Hook();
  return STATUS_SUCCESS;
}