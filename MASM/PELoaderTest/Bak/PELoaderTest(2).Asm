.386
.model flat, stdcall  ;32 bit memory model
option casemap :none  ;case sensitive

include PELoaderTest.inc

.code

start:

	invoke GetModuleHandle,NULL
	mov		hInstance,eax

    invoke InitCommonControls
	invoke DialogBoxParam,hInstance,IDD_DIALOG1,NULL,addr DlgProc,NULL
	invoke ExitProcess,0

;########################################################################

DlgProc proc hWin:HWND,uMsg:UINT,wParam:WPARAM,lParam:LPARAM
LOCAL hWnd : dword
	mov		eax,uMsg
	.if eax==WM_INITDIALOG
	       
	.elseif eax==WM_COMMAND
               mov eax, wParam
               .if  eax == IDC_BN_TEST
                     invoke MessageBox,NULL, addr g_szMsgOk, addr g_szMsgTitle, MB_OK
                     invoke MyGetProcAddress,g_pLoadMem, addr g_szMessageBox
                     lea ecx, g_szMsgTitle
                     lea ebx, g_szMsgOk
                     push MB_OK
                     push ecx
                     push ebx
                     push NULL
                     call eax
               	     ;invoke MessageBox,NULL, addr g_szMsgTxt, addr g_szMsgTitle,MB_OK
               .elseif eax == IDC_BN_FILEOP
                     mov eax, hWin
                     mov hWnd, eax
                     ;���ļ�
                     ; get a file path
	             mov g_stOfn.lStructSize,sizeof g_stOfn 
	             mov g_stOfn.lpstrFilter,offset g_szFilter
	             push hWnd
	             pop g_stOfn.hwndOwner
	             mov g_stOfn.lpstrFile, offset g_cFname 
	             mov g_stOfn.nMaxFile,SIZEOF g_cFname
	             mov g_stOfn.lpstrInitialDir,offset g_szCurDir
	             mov g_stOfn.Flags, OFN_FILEMUSTEXIST or OFN_PATHMUSTEXIST or OFN_LONGNAMES or OFN_HIDEREADONLY
	             push offset g_stOfn
                     call GetOpenFileName
                     test eax,eax
                     jz @@ExitDlgProc
                     invoke SetDlgItemText,hWnd,IDC_ED_FILE,offset g_cFname  
               .elseif eax == IDC_BN_LOAD
                     ;..�����ļ�
                     push hWin
                     pop  hWnd
                     invoke CreateFile, addr g_cFname,\
                          GENERIC_READ + GENERIC_WRITE,\
                          FILE_SHARE_READ + FILE_SHARE_WRITE,\
                          NULL,\
                          OPEN_EXISTING,\
                          FILE_ATTRIBUTE_NORMAL,\
                          0
                       .if eax == INVALID_HANDLE_VALUE
                             invoke MessageBox,hWnd, addr g_szErrorMsg, addr g_szMsgTitle, MB_OK
                             mov eax, 0
        	             jmp @@ExitDlgProc
                       .endif
                     mov g_hFile, eax
                     invoke GetFileSize,g_hFile,NULL
                     .if  eax == INVALID_FILE_SIZE
                     	     invoke MessageBox,hWnd, addr g_szErrorMsg, addr g_szMsgTitle, MB_OK
                             mov eax, 0
        	             jmp @@ExitDlgProc
                     .endif
                     mov g_dFileSize, eax
                     invoke CreateFileMapping,g_hFile, 0,PAGE_READWRITE, 0, g_dFileSize, 0
                     .if eax == 0
        	             invoke MessageBox,hWnd, addr g_szErrorMsg, addr g_szMsgTitle, MB_OK
                             mov eax, 0
        	             jmp @@ExitDlgProc
                     .endif
                      mov g_hMap, eax
                      ;���ڿ��Խ��ļ�ӳ�䵽�ڴ��� 
                      invoke MapViewOfFile,g_hMap, FILE_MAP_WRITE + FILE_MAP_READ + FILE_MAP_COPY, 0, 0, 0
                     .if eax == 0
        	             invoke MessageBox,hWnd, addr g_szErrorMsg, addr g_szMsgTitle, MB_OK
                             mov eax, 0
        	             jmp @@ExitDlgProc
                     .endif
                      mov g_pMap, eax
                      
                      invoke JudgePE,g_pMap
                     .if eax == 0
        	             invoke MessageBox,hWnd, addr g_szNotPeMsg, addr g_szMsgTitle, MB_OK
                             mov eax, 0
        	             jmp @@ExitDlgProc
                     .endif
                     ;��ȡ optionalheader��sizeofImage
                     mov esi, g_pMap
                     assume esi : ptr IMAGE_DOS_HEADER
                     add esi, [esi].e_lfanew
                     add esi, 4
                     add esi, sizeof IMAGE_FILE_HEADER
                     assume esi : ptr IMAGE_OPTIONAL_HEADER
                     mov eax, [esi].SizeOfImage
                     mov g_dImageSize, eax
                     ;���䱻װ��PE�ļ��ĵ�ַ
                     invoke VirtualAlloc, NULL, g_dImageSize, MEM_COMMIT or MEM_RESERVE, PAGE_EXECUTE_READWRITE
                     .if   eax == 0
                     	     invoke MessageBox,hWnd, addr g_szNoMem, addr g_szMsgTitle, MB_OK
                             mov eax, 0
        	             jmp @@ExitDlgProc
                     .endif   
                     mov g_pLoadMem, eax
                     invoke LoadPE,g_pLoadMem, g_pMap
                     .if eax == 0
                     	     invoke MessageBox,hWnd, addr g_szErrorMsg, addr g_szMsgTitle, MB_OK
                             mov eax, 0
        	             jmp @@ExitDlgProc
                     .endif
                     ;��Ȼ�Ѿ��ɹ����ص��ڴ������� ��ô�����ͷ� ԭ�����ڴ�ӳ���ļ���
                     invoke UnmapViewOfFile,g_pMap
                     invoke CloseHandle,g_hMap
                     invoke CloseHandle,g_hFile
                     invoke FixImport,g_pLoadMem   
                     .if eax == 0
                     	     invoke MessageBox,hWnd, addr g_szErrorMsg, addr g_szMsgTitle, MB_OK
                             mov eax, 0
        	             jmp @@ExitDlgProc
                     .endif
                     invoke FixReloc,g_pLoadMem
                     .if eax == 0
                     	     invoke MessageBox,hWnd, addr g_szErrorMsg, addr g_szMsgTitle, MB_OK
                             mov eax, 0
        	             jmp @@ExitDlgProc
                     .endif
                     invoke FindOep, g_pLoadMem
                     ;push 0
                     ;push 1	
                     ;push g_pMap
                     ;call eax
                     ;invoke VirtualFree,g_pLoadMem, g_dImageSize, MEM_RELEASE
               .else
               	     ;unknow
               .endif
	.elseif eax==WM_CLOSE
	        invoke VirtualFree,g_pLoadMem, g_dImageSize, MEM_RELEASE
		invoke EndDialog,hWin,0
	.else
		mov		eax,FALSE
		ret
	.endif
	@@ExitDlgProc:
	mov		eax,TRUE
	ret

DlgProc endp

;;****************************************************
;�ж�pMem�Ƿ���PE �ļ�
;����: pMem �ļ��ڴ�
;�Ĵ���: esi pMem , ecx ��ʱ���� 
;����ֵ: �����PE�ļ� ��eax=0
;        �����PE�ļ� ��eax=1
;----------------------------------------------------- 
JudgePE proc uses esi ecx, pMem : LPVOID
  
  xor eax, eax
  mov esi, pMem
  assume esi : ptr IMAGE_DOS_HEADER
  .if [esi].e_magic != 'ZM'
  	jmp notPE
  .endif
  ; ƫ�Ƶ�PEͷ
  mov ecx, [esi].e_lfanew
  add esi, ecx
  assume esi : ptr IMAGE_NT_HEADERS
  .if [esi].Signature != 'EP'
        jmp notPE
  .endif
  mov eax, 1 
  notPE:  
  assume esi : nothing
  ret 
JudgePE endp
;;*****************************************************

;�޸�PE�ļ��������
;������pMem --- PE�ļ����׵�ַ
;����ֵ ��������ɹ� eax = 1
;       ����         eax = 0
;ʹ�����������ǰ���� pMem�ļ��ĸ����ڶ��Ѿ������ڴ����ķ�ʽ������ 
FixImport  proc   pMem : dword
LOCAL @pImpVA         : dword  ;���뺯����
;LOCAL @pCurThunkData  : dword
LOCAL @dHModule        : dword ;��ǰIID��ģ����
LOCAL @pCurApiThunk    : dword ;��ǰThunkData(��FirstThunk��ָ��)

  mov esi, pMem
  assume esi : ptr IMAGE_DOS_HEADER
  mov eax, [esi].e_lfanew
  add esi, eax
  add esi, 04h
  add esi, sizeof IMAGE_FILE_HEADER
  assume esi : ptr IMAGE_OPTIONAL_HEADER
  mov eax, [esi].DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT * sizeof IMAGE_DATA_DIRECTORY].isize ;�˾���������
  .if  eax == 0
  	mov eax, 0
  	jmp Ext
  .endif
  
  mov eax, [esi].DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT * sizeof IMAGE_DATA_DIRECTORY].VirtualAddress
  mov esi, pMem
  add esi, eax
  mov @pImpVA, esi
  assume esi : ptr IMAGE_IMPORT_DESCRIPTOR
  .while  [esi].Name1 != 0
        ;��õ�ǰIID��module
        mov ebx, [esi].Name1
        add ebx, pMem
        invoke LoadLibrary,ebx
        .if eax == 0
                invoke MessageBox,NULL,ebx,addr g_szMsgTitle,MB_OK
        	jmp Ext
        .endif
        mov @dHModule, eax
        ;��ȡIMAGE_THUNK_DATA�б���ַ(edi)
        mov eax, [esi].FirstThunk
        add eax, pMem
        mov @pCurApiThunk, eax
        mov eax, [esi].OriginalFirstThunk
        .if  eax == 0
             mov eax, [esi].FirstThunk	
        .endif
        add eax, pMem
        mov edi, eax
        assume edi : ptr IMAGE_THUNK_DATA
        .while [edi].u1.Ordinal
             ;�ж��Ƿ���������ʽ
             mov eax, [edi].u1.AddressOfData
             shr eax, 31
             cmp eax, 1
             jz HandleOrdinal
             
             ;��������ʽ
             mov ebx, [edi].u1.AddressOfData
             add ebx, 02h
             add ebx, pMem
             jmp @f
           ;������ʽ  
           HandleOrdinal:
             mov ebx, [edi].u1.AddressOfData
             and ebx, 07FFFh
           @@:
             invoke GetProcAddress,@dHModule, ebx
             .if eax == 0
             	  jmp Ext
             .endif
             mov ebx, dword ptr[@pCurApiThunk]
             mov dword ptr [ebx], eax
             add edi, 4
             add @pCurApiThunk, 4
        .endw
        invoke FreeLibrary,@dHModule
  	add esi, sizeof IMAGE_IMPORT_DESCRIPTOR
  .endw 
  mov eax, 1
Ext:    
  ret
FixImport  endp 

;�޸�PE�ļ����ض�λ��ַ
;���� pMem       --- PE�ļ����׵�ַ
;����ֵ ��������ɹ� eax = 1
;       ����         eax = 0
;ע�� ʹ�����������ǰ���� pMem��һ���Ѿ������ص��ڴ��PE�ļ� ����ͬʱҲҪ����û���ض�λ�ڵ���� 
FixReloc   proc   pMem : dword
LOCAL @dRelocBaseRVA   : dword ;�ض�λ���ݿ�ʼ��RVA
LOCAL @dRelocItemCount : dword ;�ض�λ�������
;LOCAL @dwTypeOffset    : word  ;��ǰ��TypeOffset 
LOCAL @dBaseCmpF       : dword ;������ַ�Ĵ�С�ȽϽ�� 
LOCAL @dImageBaseOffset : dword ;Ĭ�ϵ�ImageBase�͵�ǰ��ImageBase��offset

  ;��λ���ض�λ������Ŀ¼�� 
  mov esi, pMem
  assume esi : ptr IMAGE_DOS_HEADER
  add esi, [esi].e_lfanew
  add esi, 04h
  add esi, sizeof IMAGE_FILE_HEADER
  assume esi : ptr IMAGE_OPTIONAL_HEADER
  ;����imagebase֮��
  mov eax, [esi].ImageBase
  .if eax < pMem ;����
  	mov eax, 1
  	mov @dBaseCmpF, eax
  	mov ebx, pMem
  	mov eax, [esi].ImageBase
  	xchg eax, ebx
  	sub eax, ebx
  .elseif eax > pMem ;����
  	mov eax, 0
  	mov @dBaseCmpF, eax
  	mov eax, [esi].ImageBase
  	sub eax, pMem
  .else 
  	mov eax, 2   ;�������Ϊ�� 
  	mov @dBaseCmpF, eax
  	mov eax, 0
  .endif
  mov @dImageBaseOffset, eax
  ;��λ���ض�λ��
  mov eax, [esi].DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC * sizeof IMAGE_DATA_DIRECTORY].VirtualAddress
  test eax, eax
  jnz @f
  mov eax, 0
  jmp Ext
 @@:
  mov esi, pMem
  add esi, eax
  assume esi : ptr IMAGE_BASE_RELOCATION
  mov eax, @dBaseCmpF
  cmp eax, 1
  jz  HandleCmpPostive
  cmp eax, 0
  jz  HandleCmpNegtive
 HandleCmpPostive: 
  .while  [esi].VirtualAddress != 0
  	mov eax, [esi].SizeOfBlock
  	sub eax, 8
  	shr eax, 1 ;����2
  	mov @dRelocItemCount, eax
  	
  	mov ecx, eax
  	mov edi, esi
  	add edi, 8
      LoopTypeOffset0:
        mov  ax, word ptr [edi]
        shr ax, 12
        .if   ax == IMAGE_REL_BASED_HIGHLOW
        	mov ax, word ptr [edi]
        	and ax, 0FFFh
        	movzx eax, ax
        	add eax, [esi].VirtualAddress
        	mov ebx, eax
        	add ebx, pMem
        	mov eax, @dImageBaseOffset
        	add dword ptr[ebx], eax
        .endif
        add edi, 2
        loop LoopTypeOffset0
  	
  	mov eax, @dRelocItemCount
  	shl eax, 1 ;����2
  	add esi, sizeof IMAGE_BASE_RELOCATION
  	add esi, eax
  .endw
  jmp SucExt
 HandleCmpNegtive:
  .while  [esi].VirtualAddress != 0
  	mov eax, [esi].SizeOfBlock
  	sub eax, 8
  	shr eax, 1 ;����2
  	mov @dRelocItemCount, eax
  	
  	mov ecx, eax
  	mov edi, esi
  	add edi, 8
      LoopTypeOffset1:
        mov  ax, word ptr [edi]
        shr ax, 12
        .if   ax == IMAGE_REL_BASED_HIGHLOW
        	mov ax, word ptr [edi]
        	and ax, 0FFFh
        	movzx eax, ax
        	add eax, [esi].VirtualAddress
        	mov ebx, eax
        	add ebx, pMem
        	mov eax, @dImageBaseOffset
        	sub dword ptr[ebx], eax
        .endif
        add edi, 2
        loop LoopTypeOffset1
  	
  	mov eax, @dRelocItemCount
  	shl eax, 1 ;����2
  	add esi, sizeof IMAGE_BASE_RELOCATION
  	add esi, eax
  .endw  
SucExt:  
  mov eax, 1
Ext:
  ret
FixReloc endp

;�ҵ�PE�ļ�����ڵ�
;���� pMem : PE�ļ����ڴ��еĵ�ַ
;ע�� ���������ʹ��ǰ���� pMem��ַ��Pe�ļ� �Ѿ������ڴ���뷽ʽ����
FindOep     proc   pMem : dword                    
  mov esi, pMem
  assume esi : ptr IMAGE_DOS_HEADER
  add esi, [esi].e_lfanew
  add esi, 4
  add esi, sizeof IMAGE_FILE_HEADER
  assume esi : ptr IMAGE_OPTIONAL_HEADER
  mov eax, [esi].AddressOfEntryPoint	
  add eax, pMem	
  ret
FindOep endp

;����PE�ļ����ڴ�
;���� pMem       --- ����������ڴ�ռ��ַ
;     pPEFile    --- PE�ļ����ڵĿռ�  
;����ֵ ������سɹ� eax = 1
;       ����         eax = 0
LoadPE     proc   pMem : dword, pPEFile : dword
LOCAL @dSecCount  : dword
LOCAL @pSecHeader : dword
;LOCAL @dOEP       : dword ;��ڵ� 

  mov esi, pPEFile
  mov edi, pMem
  ;;����PEͷ
  assume esi : ptr IMAGE_DOS_HEADER
  mov eax, [esi].e_lfanew
  add esi, eax
  add esi, 4
  assume esi : ptr IMAGE_FILE_HEADER
  ;��ȡ����Ŀ
  mov ax, [esi].NumberOfSections
  movzx eax, ax
  mov @dSecCount, eax
  ;��ȡ�׸���ͷ��ַ
  mov cx, [esi].SizeOfOptionalHeader
  movzx ecx, cx
  mov ebx, esi
  add ebx, ecx
  add ebx, sizeof IMAGE_FILE_HEADER
  ;add ebx, pMem
  mov @pSecHeader, ebx
  ;������λ
  add esi, sizeof IMAGE_FILE_HEADER
  assume esi : ptr IMAGE_OPTIONAL_HEADER
  mov eax, [esi].AddressOfEntryPoint
  ;mov @dOEP, eax
  mov ecx, [esi].SizeOfHeaders
  mov esi, pPEFile
LoopMovPEHeaders: 
  lodsb
  stosb
  loop LoopMovPEHeaders
  ;;���Ƹ���������
  ;����׸������ݵ�FileOffset
  mov ebx, @pSecHeader
  assume ebx : ptr IMAGE_SECTION_HEADER
  mov ecx, @dSecCount
LoopMovSec:
  mov esi, [ebx].PointerToRawData
  add esi, pPEFile
  mov edi, [ebx].VirtualAddress
  add edi, pMem
  
  push ecx
  mov ecx, [ebx].SizeOfRawData
  .if ecx == 0
  	jmp @f
  .endif
  LoopMovSecData:
  lodsb
  stosb
  loop LoopMovSecData
 @@:
  pop ecx
  add ebx, sizeof IMAGE_SECTION_HEADER
  loop LoopMovSec
  mov eax, 1
Ext:
  ret
LoadPE endp

;;������ʽ�Զ����GetProcAddress���� ʵ��GetProcAddress�����Ĺ���
;����:  hModule -- DLLģ����(ʵ������ģ���ַ)
;       dFuncName -- �������ĵ�ַ
;����ֵ ������óɹ� eax = dFuncName�����ĵ�ַ
;       ���ʧ��     eax = 0
MyGetProcAddress  proc   hModule : dword, dFuncName : dword
LOCAL @dBaseOrdinal : dword   
LOCAL @pMem         : dword
LOCAL @pENT         : dword
LOCAL @pEAT         : dword
LOCAL @pEOT         : dword 
LOCAL @dIndex       : dword

   mov eax, hModule
   mov @pMem, eax
   mov esi, hModule
   assume esi : ptr IMAGE_DOS_HEADER
   add esi, [esi].e_lfanew
   add esi, 4
   add esi, sizeof IMAGE_FILE_HEADER
   assume esi : ptr IMAGE_OPTIONAL_HEADER
   mov esi, [esi].DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT * sizeof IMAGE_DATA_DIRECTORY].VirtualAddress
   add esi, @pMem
   assume esi : ptr IMAGE_EXPORT_DIRECTORY
   mov eax, [esi].nBase
   mov @dBaseOrdinal, eax
   mov eax, [esi].AddressOfNames
   add eax, @pMem
   mov @pENT, eax
   mov eax, [esi].AddressOfFunctions
   add eax, @pMem
   mov @pEAT, eax
   mov eax, [esi].AddressOfNameOrdinals
   add eax, @pMem
   mov @pEOT, eax
   ;�ж�����Ż��Ǻ��� Ѱַ�ķ�ʽ
   mov eax, dFuncName
   and eax, 0FFFF0000h
   jz @f
   jmp HandleFuncnameProc
   @@:
   mov ebx, dFuncName
   jmp HandleOrdinalProc
   
   ;���ú�������ѯ
 HandleFuncnameProc: 
   xor eax, eax
   mov @dIndex, eax
   mov ecx, [esi].NumberOfNames
   sub ecx, @dBaseOrdinal
   inc ecx
   mov esi, [esi].AddressOfNames
   add esi, @pMem
  QueryLoop:
   push ecx
   inc @dIndex
   mov ebx, dword ptr [esi]
   add ebx, @pMem
   invoke StrCmp,ebx, dFuncName
   test eax, eax
   jz @f
   add esi, 4
   pop ecx
   loop QueryLoop
   
   mov eax, 0
   jmp Ext
   @@:
   ;��ú����������� 
   mov eax, @dIndex
   mov esi, @pEOT
   xor edx, edx
   imul eax, 2
   add esi, eax
   movzx ebx, word ptr [esi]
   
   ;����������ѯ 
  HandleOrdinalProc:
   sub ebx, @dBaseOrdinal
   inc ebx
   xor edx, edx
   imul ebx, 4
   
   mov esi, @pEAT
   add esi, ebx
   mov eax,  dword ptr[esi]
   add eax, @pMem
 Ext:  
   ret
MyGetProcAddress  endp  

end start