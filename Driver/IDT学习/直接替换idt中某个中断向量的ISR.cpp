/*
直接替换idt中某个中断向量的ISR.
这里采用的办法是，首先保存出中断向量0x30的ISR, 
然后直接修改中断向量0x30的ISR为我们自定义的函数 my_interrupt_hook，
每当0x30号中断产生，就会调用我们自定义的函数。
由于我们自定义的函数里面执行完我们的功能后再跳转到原isr函数处执行。
因此这种hook用户感觉不到，不影响原有功能。在OnUnload时，恢复原有的中断向量ISR
*/
#include "ntddk.h"
#include <stdio.h>

#define MAKELONG(a, b) ((unsigned long) (((unsigned short) (a)) | ((unsigned long) ((unsigned short) (b))) << 16)) 

#define MAX_IDT_ENTRIES 0xFF
#define NT_INT_TIMER        0x30

unsigned long g_i_count = 0;

///////////////////////////////////////////////////
// IDT structures
///////////////////////////////////////////////////
#pragma pack(1)

// entry in the IDT, this is sometimes called
// an "interrupt gate"
typedef struct
{
  unsigned short LowOffset;
  unsigned short selector;
  unsigned char unused_lo;
  unsigned char segment_type:4;  //0x0E is an interrupt gate
  unsigned char system_segment_flag:1;
  unsigned char DPL:2;         // descriptor privilege level 
  unsigned char P:1;            /* present */
  unsigned short HiOffset;
} IDTENTRY;

/* sidt returns idt in this format */
typedef struct
{
  unsigned short IDTLimit;
  unsigned short LowIDTbase;
  unsigned short HiIDTbase;
} IDTINFO;

#pragma pack()

unsigned long old_ISR_pointer;  // better save the old one!!


VOID OnUnload( IN PDRIVER_OBJECT DriverObject )
{  
  IDTINFO    idt_info;    // this structure is obtained by calling STORE IDT (sidt)
  IDTENTRY*  idt_entries;  // and then this pointer is obtained from idt_info
  char _t[255];

  // load idt_info
  __asm  sidt  idt_info  
  idt_entries = (IDTENTRY*) MAKELONG(idt_info.LowIDTbase,idt_info.HiIDTbase);

  DbgPrint("ROOTKIT: OnUnload called\n");

  _snprintf(_t, 253, "called %d times", g_i_count);
  DbgPrint(_t);
  
  DbgPrint("UnHooking Interrupt...");

  // restore the original interrupt handler
  __asm cli
  idt_entries[NT_INT_TIMER].LowOffset = (unsigned short) old_ISR_pointer;
  idt_entries[NT_INT_TIMER].HiOffset = (unsigned short)((unsigned long)old_ISR_pointer >> 16);
  __asm sti

  DbgPrint("UnHooking Interrupt complete.");
}

// using stdcall means that this function fixes the stack before returning (opposite of cdecl)
void __stdcall count_syscall( unsigned long system_call_number )
{
  g_i_count++;
}

// naked functions have no prolog/epilog code - they are functionally like the 
// target of a goto statement
__declspec(naked) my_interrupt_hook()
//就是告诉编译器，在编译的时候，不要优化代码，通俗的说就是
//没代码，完全要自己写
{
	DbgPrint(("函数HOOK"));
  __asm
  {
    push  eax
    call  count_syscall
    jmp    old_ISR_pointer
  }
}

extern "C" NTSTATUS DriverEntry( IN PDRIVER_OBJECT theDriverObject, IN PUNICODE_STRING theRegistryPath )
{
  IDTINFO    idt_info;    // this structure is obtained by calling STORE IDT (sidt)
  IDTENTRY*  idt_entries;  // and then this pointer is obtained from idt_info
  IDTENTRY*  i;
  unsigned long   addr;
  unsigned long  count;
  char _t[255];
  
  theDriverObject->DriverUnload  = OnUnload; 

  // load idt_info
  __asm  sidt  idt_info
  
  idt_entries = (IDTENTRY*) MAKELONG(idt_info.LowIDTbase,idt_info.HiIDTbase);

  for(count=0;count < MAX_IDT_ENTRIES;count++)
  {
    i = &idt_entries[count];
    addr = MAKELONG(i->LowOffset, i->HiOffset);
    
    _snprintf(_t, 253, "Interrupt %d: ISR 0x%08X", count, addr);
    DbgPrint(_t);
  }

  DbgPrint("Hooking Interrupt...");
  // lets hook an interrupt
  // exercise - choose your own interrupt
  old_ISR_pointer = MAKELONG(idt_entries[NT_INT_TIMER].LowOffset,idt_entries[NT_INT_TIMER].HiOffset);
  
// debug, use this if you want some additional info on what is going on
#if 1//0
  _snprintf(_t, 253, "old address for ISR is 0x%08x", old_ISR_pointer);
  DbgPrint(_t);
  _snprintf(_t, 253, "address of my function is 0x%08x", my_interrupt_hook);
  DbgPrint(_t);
#endif
  
  // remember we disable interrupts while we patch the table
  __asm cli
  idt_entries[NT_INT_TIMER].LowOffset = (unsigned short)my_interrupt_hook;
  idt_entries[NT_INT_TIMER].HiOffset = (unsigned short)((unsigned long)my_interrupt_hook >> 16);
  __asm sti

// debug - use this if you want to check what is now placed in the interrupt vector
#if 1//0
  i = &idt_entries[NT_INT_TIMER];
  addr = MAKELONG(i->LowOffset, i->HiOffset);
  _snprintf(_t, 253, "Interrupt ISR 0x%08X", addr);
  DbgPrint(_t);  
#endif

  DbgPrint("Hooking Interrupt complete");

  return STATUS_SUCCESS;
}