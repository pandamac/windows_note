// udp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <stdio.h>
#pragma comment (lib, "ws2_32")

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET s = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    sockaddr_in sockAddr;
    sockAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(827);
    sockAddr.sin_family = PF_INET;

    bind(s, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

    sockaddr_in clientAddr;
    int len = sizeof(sockaddr_in);
    char buf[MAXBYTE] = { 0 };
    recvfrom(s, buf, MAXBYTE, 0, (SOCKADDR*)&clientAddr, &len);
    printf("%s \r\n", buf);
    
    sendto(s, "world", strlen("world") + sizeof(char), 0, (SOCKADDR*)&clientAddr, sizeof(SOCKADDR));
    closesocket(s);

    WSACleanup();
	return 0;
}