// syn_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string.h>
#include <iostream>

#include <winsock2.h>
#pragma comment(lib,"WS2_32")

#include <WS2TCPIP.h>
#include <time.h>

// New WSAIoctl Options
#define SIO_RCVALL            _WSAIOW(IOC_VENDOR,1)
#define SIO_RCVALL_MCAST      _WSAIOW(IOC_VENDOR,2)
#define SIO_RCVALL_IGMPMCAST  _WSAIOW(IOC_VENDOR,3)
#define SIO_KEEPALIVE_VALS    _WSAIOW(IOC_VENDOR,4)
#define SIO_ABSORB_RTRALERT   _WSAIOW(IOC_VENDOR,5)
#define SIO_UCAST_IF          _WSAIOW(IOC_VENDOR,6)
#define SIO_LIMIT_BROADCASTS  _WSAIOW(IOC_VENDOR,7)
#define SIO_INDEX_BIND        _WSAIOW(IOC_VENDOR,8)
#define SIO_INDEX_MCASTIF     _WSAIOW(IOC_VENDOR,9)
#define SIO_INDEX_ADD_MCAST   _WSAIOW(IOC_VENDOR,10)
#define SIO_INDEX_DEL_MCAST   _WSAIOW(IOC_VENDOR,11) 


typedef struct _iphdr 
{
	unsigned char h_lenver; //4位首部长度+4位IP版本号
	unsigned char tos; //8位服务类型TOS
	unsigned short total_len; //16位总长度（字节）
	unsigned short ident; //16位标识
	unsigned short frag_and_flags; //3位标志位
	unsigned char ttl; //8位生存时间 TTL
	unsigned char proto; //8位协议 (TCP, UDP 或其他)
	unsigned short checksum; //16位IP首部校验和
	unsigned int sourceIP; //32位源IP地址
	unsigned int destIP; //32位目的IP地址
}IP_HEADER;

typedef struct _tcphdr //定义TCP首部
{
	USHORT th_sport; //16位源端口
	USHORT th_dport; //16位目的端口
	unsigned int th_seq; //32位序列号
	unsigned int th_ack; //32位确认号
	unsigned char th_lenres; //4位首部长度/6位保留字
	unsigned char th_flag; //6位标志位
	USHORT th_win; //16位窗口大小
	USHORT th_sum; //16位校验和
	USHORT th_urp; //16位紧急数据偏移量
}TCP_HEADER; 

struct //定义TCP伪首部
{
	unsigned long saddr; //源地址
	unsigned long daddr; //目的地址
	char mbz;
	char ptcl; //协议类型
	unsigned short tcpl; //TCP长度
}psd_header;

SOCKET sockRaw = INVALID_SOCKET,sockListen = INVALID_SOCKET;
int BeginPort,EndPort;
char *HOST;
int iErrorCode;
struct sockaddr_in dest;
BOOL StopScan = FALSE;

#define SEQ 0x28376839

void CheckSockError(int ierror,char *pErrorMsg)
{
	if (ierror == SOCKET_ERROR)
	{
		printf("%s ErrorCode:%d\n",pErrorMsg,ierror);
		closesocket(sockRaw);
		ExitProcess(-1);
	}
}


void meesage()
{
	printf("\t-------syn by panda--------------------------\n");
	printf("\t-------syn_test [ip] [port-port]-------------\n");
	printf("\t-------example: syn_test 127.0.0.1 1-1000----\n");
}
BOOL DecodeIPHeader(char *RecvBuf)
{
	IP_HEADER *iphdr = (IP_HEADER*)RecvBuf;
	unsigned short iphdrlen = sizeof(unsigned long) * (iphdr->h_lenver&0xf);
	TCP_HEADER* tcphdr = (TCP_HEADER*)(RecvBuf + iphdrlen);
	
	if (iphdr->sourceIP != dest.sin_addr.s_addr)
	{
		return FALSE;
	}
	if (ntohl(tcphdr->th_ack) != (SEQ+1) && ntohl(tcphdr->th_ack) != SEQ)
	{
		return FALSE;
	}
	if (tcphdr->th_flag == 18)
	{
		printf("\t%d open \n",ntohs(tcphdr->th_sport));
		return true;
	}
	return FALSE;
}


DWORD WINAPI RecvThread(LPVOID para)//接受数据线程
{
	//监听本机套接字
	sockListen = socket(AF_INET,SOCK_RAW,IPPROTO_IP);
	CheckSockError(sockListen,"RecvThread` socket");
	
	BOOL bOpt =true;
	iErrorCode = setsockopt(sockRaw,IPPROTO_IP,IP_HDRINCL,(char*)&bOpt,sizeof(bOpt));
	CheckSockError(iErrorCode,"RecvThread setsockopt");
	
	//获得本地IP
	char LocalName[256];
	gethostname(LocalName,sizeof(LocalName));
	struct hostent * my_hostent = gethostbyname(LocalName);
	
	SOCKADDR_IN sa;
	memcpy(&sa.sin_addr.S_un.S_addr,my_hostent->h_addr_list[0],my_hostent->h_length);
	sa.sin_family = AF_INET;
	sa.sin_port = htons(8000);
	
	iErrorCode = bind(sockListen,(sockaddr*)&sa,sizeof(sa));
	CheckSockError(iErrorCode,"bind");

	//设置SOCK_RAW为SIO_RCVALL，以便接收所有的IP包
	DWORD dwBufferInLen = 1;
	DWORD dwBufferLen[10];
	DWORD dwBytesReturned = 0;
	iErrorCode = WSAIoctl(sockListen,SIO_RCVALL,\
		&dwBufferInLen,sizeof(dwBufferInLen),\
		&dwBufferLen,sizeof(dwBufferLen),&dwBytesReturned,NULL,NULL);
	CheckSockError(iErrorCode,"RecvThread WSAIoctl");
	
	char RecvBuf[65535]={0};
	memset(RecvBuf,0,sizeof(RecvBuf));
	
	while (1)//循环监听  本地 数据包
	{
		iErrorCode = recv(sockListen,RecvBuf,sizeof(RecvBuf),0);
		//CheckSockError(iErrorCode,"RecvThread  recv");
		DecodeIPHeader(RecvBuf);
		if (StopScan == TRUE)
		{
			closesocket(sockListen);
			return 0;
		}
	}
	return 0;
}



USHORT CalcCheckSum(USHORT *buffer,int size)
{
	unsigned long cksum = 0;
	while (size > 1)
	{
		cksum += *buffer++;
		size -= sizeof(USHORT);
	}
	if (size)
	{
		cksum += *(USHORT*)buffer;
	}
	cksum = (cksum >> 16) + (cksum &0xffff);
	cksum += (cksum >>16);
	return (USHORT)(~cksum);
}
int play=0;
void progressbar(void)
{
	// 进度条 
	char *plays[12]= 
	{ 
		" | ", 
			" / ", 
			" - ", 
			" \\ ", 
			" | ", 
			" / ", 
			" - ", 
			" \\ ", 
			" | ", 
			" / ", 
			" - ", 
			" \\ ", 
	}; 
	printf(" =%s=\r", plays[play]);
	play = (play==11)?0:play+1;
	Sleep(2); 
}
int main(int argc, char* argv[])
{
	char *p;
	if (argc != 3)
	{
		meesage();
		return 0;
	}
	p = argv[2];
	if (strstr(argv[2],"-"))
	{
		BeginPort = atoi(argv[2]);
		while (*p)
		{
			if (*(p++) == '-')
			{
				break;
			}
		}
		EndPort = atoi(p);
		if (BeginPort <1 || BeginPort>65535 ||EndPort<1|| EndPort >65535|| EndPort<EndPort)
		{
			meesage();
			return 0;
		}
	}
	HOST = argv[1];
	meesage();

	WSADATA wsadata;
	iErrorCode = WSAStartup(MAKEWORD(2,2),&wsadata);
	CheckSockError(iErrorCode, "WsaStartup()");

	//////////////////////////////////////////////////////////////////////////////
	sockRaw = socket(AF_INET,SOCK_RAW,IPPROTO_IP);
	CheckSockError(sockRaw, "socket()");
	//设置IP头操作选项 是发送TCP报文的套接字
	BOOL bOpt = true;
	setsockopt(sockRaw,IPPROTO_IP,IP_HDRINCL,(char*)&bOpt,sizeof(bOpt));
	CheckSockError(sockRaw,"setsockopt()");

	//获得目标主机IP ,通过发送主机  第一次握手包
	memset(&dest,0,sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = ntohs(BeginPort);

	struct hostent *my_hostent;
	if ((dest.sin_addr.s_addr = inet_addr(HOST)) == INADDR_NONE)
	{
		if ((my_hostent = gethostbyname(HOST)) == NULL)
		{
			memcpy(&(dest.sin_addr),my_hostent->h_addr_list[0],my_hostent->h_length);
			dest.sin_family = my_hostent->h_addrtype;
			printf("dest.sin_addr = %s",inet_ntoa(dest.sin_addr));
		}
		else
		{
			CheckSockError(SOCKET_ERROR,"gethostbyname");
		}
	}
	//////////////////////////////////////////////////////////////////////////////
	sockListen = socket(AF_INET , SOCK_RAW , IPPROTO_IP);
	CheckSockError(sockListen, "socket");
	
	//获得本地IP
	SOCKADDR_IN sa;
	unsigned char LocalName[256];
	struct hostent *hp;
	
	iErrorCode = gethostname((char*)LocalName, sizeof(LocalName)-1);
	CheckSockError(iErrorCode, "gethostname()");
	if((hp = gethostbyname((char*)LocalName)) == NULL)
	{
		CheckSockError(SOCKET_ERROR, "gethostbyname()");
	}
	memcpy(&sa.sin_addr.S_un.S_addr, hp->h_addr_list[0],hp->h_length);
	sa.sin_family = AF_INET;
	sa.sin_port = htons(8000);
	iErrorCode = bind(sockListen, (PSOCKADDR)&sa, sizeof(sa));
	CheckSockError(iErrorCode, "bind");

	//开启本地监听 第二次握手包 线程
	HANDLE Thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)RecvThread,0,0,0);
	Sleep(1000);
    //////////////////////////////////////////////////////////////////////////////
	//发送第一次握手包
	
	IP_HEADER ip_header;
	TCP_HEADER tcp_header;
	//填充IP首部 一个IP包头的长度最长为“1111”，即15*4＝60个字节。IP包头最小长度为20字节。对于标准ipv4报头，这个字段的值肯定是20/4=5（10进制）=0101（2进制）。
	ip_header.h_lenver = (4<<4 | sizeof(ip_header)/sizeof(unsigned long));
	//高四位IP版本号，低四位首部长度
	ip_header.total_len=htons(sizeof(IP_HEADER)+sizeof(TCP_HEADER)); //16位总长度（字节）
	ip_header.ident=1; //16位标识
	ip_header.frag_and_flags=0; //3位标志位
	ip_header.ttl=128; //8位生存时间TTL
	ip_header.proto=IPPROTO_TCP; //8位协议(TCP,UDP…)
	ip_header.checksum=0; //16位IP首部校验和
	ip_header.sourceIP=sa.sin_addr.s_addr;  //32位源IP地址
	ip_header.destIP=dest.sin_addr.s_addr;  //32位目的IP地址
	
	//填充TCP首部
	tcp_header.th_sport = htons(8000);//源端口号
	tcp_header.th_lenres = (sizeof(TCP_HEADER)/4<<4 | 0);//TCP长度和保留位
	tcp_header.th_win=htons(16384); 

	//填充TCP伪首部（用于计算校验和，并不真正发送）
	psd_header.saddr=ip_header.sourceIP;
	psd_header.daddr=ip_header.destIP;
	psd_header.mbz=0;
	psd_header.ptcl=IPPROTO_TCP;
	psd_header.tcpl=htons(sizeof(tcp_header));

	
	printf("\n");
	printf("Scaning %s port : %d-%d\n",HOST,BeginPort,EndPort);
	clock_t start,end;//程序运行的起始和结束时间

	  start=clock();//开始计时
	//开始发包~~~~
	  char SendBuf[128] = {0};
	for (;BeginPort < EndPort;BeginPort++)
	{
		// 进度条 
		progressbar();
		
		tcp_header.th_dport = htons(BeginPort); //目的端口号
		tcp_header.th_ack=0;                  //ACK序列号置为0
		tcp_header.th_flag=2;                 //SYN 标志
		tcp_header.th_seq=htonl(SEQ);         //SYN序列号
		tcp_header.th_urp=0;                  //偏移
		tcp_header.th_sum=0;                  //校验和
		
		
		
		//计算TCP校验和 即TCP头部和TCP数据进行校验和计算，并由目标端进行验证。
		memcpy(SendBuf,&psd_header,sizeof(psd_header));
		memcpy(SendBuf+sizeof(psd_header), &tcp_header,sizeof(tcp_header));
		tcp_header.th_sum = CalcCheckSum((USHORT *)SendBuf,sizeof(psd_header)+sizeof(tcp_header));
		
		//计算IP校验和 IP包头是变长的，所以提供一个头部校验来保证IP包头中信息的正确性。
		memcpy(SendBuf,&ip_header,sizeof(ip_header));
		memcpy(SendBuf+sizeof(ip_header),&tcp_header,sizeof(tcp_header));
		memset(SendBuf+sizeof(ip_header)+sizeof(tcp_header),0,4);
		ip_header.checksum = CalcCheckSum((USHORT *)SendBuf,sizeof(ip_header)+sizeof(tcp_header));

		//填充发送缓冲区
		memcpy(SendBuf,&ip_header,sizeof(ip_header));

		//发送TCP报文
		iErrorCode=sendto(sockRaw,SendBuf,sizeof(ip_header)+sizeof(tcp_header),0,\
			(struct sockaddr*) &dest,
			sizeof(dest));
		CheckSockError(iErrorCode, "sendto()");
	}
	//结束发包~~~~
	end=clock();//计时结束
	StopScan = TRUE;
	printf("Closeing Scan.....\n");
	WaitForSingleObject(Thread,5000);
	CloseHandle(Thread);

	printf("Cost time: %f Sec",(float)(end-start) / CLOCKS_PER_SEC/*1000*/);
	
	if (sockRaw != INVALID_SOCKET)
	{
		closesocket(sockRaw);
	}
	if (sockListen!= INVALID_SOCKET)
	{
		closesocket(sockListen);
	}
	WSACleanup();
	return 0;
}

