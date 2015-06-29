#include<stdio.h>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib")
void CheckIP(void) //定义CheckIP（）函数，用于获取本机IP地址 
{
WSADATA wsaData;
char name[255];//定义用于存放获得的主机名的变量 
char *ip;//定义IP地址变量 
PHOSTENT hostinfo; 
//调用MAKEWORD（）获得Winsock版本的正确值，用于加载Winsock库 
if ( WSAStartup( MAKEWORD(2,0), &wsaData ) == 0 ) { 
//现在是加载Winsock库，如果WSAStartup（）函数返回值为0，说明加载成功，程序可以继续 
if( gethostname ( name, sizeof(name)) == 0) { 
//如果成功地将本地主机名存放入由name参数指定的缓冲区中 
if((hostinfo = gethostbyname(name)) != NULL) { 
//这是获取主机名，如果获得主机名成功的话，将返回一个指针，指向hostinfo，hostinfo 
//为PHOSTENT型的变量，下面即将用到这个结构体 
ip = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list); 
//调用inet_ntoa（）函数，将hostinfo结构变量中的h_addr_list转化为标准的点分表示的IP 
//地址（如192.168.0.1） 
printf("%s\n",ip);//输出IP地址
 } 
} 
WSACleanup( );//卸载Winsock库，并释放所有资源 
} } 
int main(void)
{
CheckIP();
return 0;
}
