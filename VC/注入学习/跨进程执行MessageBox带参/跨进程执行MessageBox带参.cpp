#include <windows.h>
#include <iostream.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <iostream.h>

struct RemoteStruct //远程结构体
{
	char MyText[255];		//内容
	char MyCaption[255];	//标题
	DWORD Address;			//存放重定位以后的MessageBox地址
};

typedef int (WINAPI*MyMessageBox)(HWND,LPCTSTR,LPCTSTR,UINT); //函数原型

void WINAPI MyThread(void *MyArg) //远程进程
{
	RemoteStruct *MyRemoteStruct=(RemoteStruct*)MyArg;
	MyMessageBox My;
	My=(MyMessageBox)MyRemoteStruct->Address; //指向MessageBox地址
	My(NULL,MyRemoteStruct->MyText,MyRemoteStruct->MyCaption,MB_OK); //调用函数
}

void ProcessList()
{
	//列出进程名与对应的Pid
	HANDLE SnapshotHandle=NULL; //快照句柄
	SnapshotHandle=::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); //创建进程快照
	if(SnapshotHandle==NULL)
	{
		system("cls");
		cout<<"错误："<<endl;
		cout<<"创建进程快照失败。"<<endl;
		system("pause");
		exit(1);
	}
	cout<<"ProcessName:                   PID:"<<endl;
	BOOL All=FALSE; //是否全部遍历出来了
	PROCESSENTRY32W Mylppe;
	Mylppe.dwSize=sizeof(Mylppe); //设置大小

	All=::Process32FirstW(SnapshotHandle,&Mylppe); //取得第一个进程快照
	while(All) //循环取出
	{
		for (int i = 0;Mylppe.szExeFile[i]!='\0';i++)
		{
			printf("%c",Mylppe.szExeFile[i]);
		}
		cout<<"                      "<<Mylppe.th32ProcessID<<endl;
		All=::Process32NextW(SnapshotHandle,&Mylppe); 
		//取得下个
	}
	CloseHandle(SnapshotHandle); //释放快照句柄
}

int main()
{
	DWORD Pid=0;
	char FunctionText[255]={NULL}; //内容
	char FunctionCaption[255]={NULL}; //标题
	
	LPVOID lpArgBuf=NULL; //申请的空间(参数)
	LPVOID lpFunBuf=NULL; //申请的空间(函数)
	
	DWORD WriteByteArg=0; //参数实际写入字节数
	DWORD WriteByteFun=0; //函数实际写入字节数
	
	RemoteStruct MyRemoteStruct;
	while(1)
	{
		cout<<"------------------------------------"<<endl;
		cout<<"下面是进程，请选择你要注入的进程，并输入它的Pid。"<<endl;
		ProcessList(); //遍历进程
		cout<<endl; //空行
		cout<<"请输入要跨的进程ID号(Pid)："<<endl;
		cin>>Pid;
		if(Pid==0)
		{
			system("cls");
			cout<<"错误："<<endl;
			cout<<"请确认你输入的Pid是正确的！"<<endl;
			system("pause");
			exit(1);
		}
		//打开进程
		HANDLE ProcessHandle=NULL;
		ProcessHandle=::OpenProcess(PROCESS_ALL_ACCESS,FALSE,Pid); //打开进程
		if(ProcessHandle == NULL) //失败
		{
			system("cls");
			cout<<"错误："<<endl;
			cout<<"不能打开进程，请确认你输入的Pid是正确的！"<<endl;
			system("pause");
			exit(1);
		}
		//取得MessageBox的参数
		cout<<"打开进程成功，请输入要弹框的内容："<<endl;
		cin>>FunctionText;
		cout<<"请输入要弹框的标题："<<endl;
		cin>>FunctionCaption;
		//在指定进程内申请空间放置这两个参数
		strcpy(MyRemoteStruct.MyText,FunctionText);
		strcpy(MyRemoteStruct.MyCaption,FunctionCaption);

		MyRemoteStruct.Address=(DWORD)MessageBox; //取得函数地址

		lpArgBuf=::VirtualAllocEx(ProcessHandle,0,1024,MEM_COMMIT,PAGE_EXECUTE_READWRITE); //申请空间并记录空间
		if(lpArgBuf==NULL) //失败
		{
			system("cls");
			cout<<"错误："<<endl;
			cout<<"不能在此进程内申请空间(参数)！"<<endl;
			system("pause");
			exit(1);
		}
if(!WriteProcessMemory(ProcessHandle,lpArgBuf,&MyRemoteStruct,sizeof(MyRemoteStruct),&WriteByteArg))
 //写入参数
		{
			system("cls");
			cout<<"错误："<<endl;
			cout<<"不能写入参数！"<<endl;
			system("pause");
			exit(1);
		}
		//分配内存写入线程函数
		lpFunBuf=::VirtualAllocEx(ProcessHandle,0,4096,MEM_COMMIT,PAGE_EXECUTE_READWRITE); //函数地址
		if(lpFunBuf==NULL)
		{
			system("cls");
			cout<<"错误："<<endl;
			cout<<"不能在此进程内申请空间(函数)！"<<endl;
			system("pause");
			exit(1);
		}
		//写入函数(大小为4096)
		if(!WriteProcessMemory(ProcessHandle,lpFunBuf,&MyThread,4096,&WriteByteFun))
		{
			system("cls");
			cout<<"错误："<<endl;
			cout<<"不能写入函数！"<<endl;
			system("pause");
			exit(1);
		}
		//远程执行函数(线程)
		DWORD ThreadId=0; 
		//远程线程Id
		HANDLE hThread=::CreateRemoteThread(ProcessHandle,0,0,\
			(LPTHREAD_START_ROUTINE)lpFunBuf,lpArgBuf,0,&ThreadId);
		//执行函数
		::WaitForSingleObject(hThread,INFINITE); //等待远程函数执行完毕
		VirtualFreeEx(ProcessHandle,lpFunBuf,4096,MEM_COMMIT); //释放函数空间
		VirtualFreeEx(ProcessHandle,lpArgBuf,1024,MEM_COMMIT); //释放参数空间
		CloseHandle(ProcessHandle);
		cout<<"注入成功！"<<endl;
		system("pause");
		system("cls");
	}
	return 0;
}