// ping.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>  
#include <WS2tcpip.h>  
#include <stdio.h>  
#include <stdlib.h>  
  
#pragma comment(lib,"WS2_32")
#define ICMP_ECHO 8  
#define ICMP_ECHOREPLY 0  
#define ICMP_MIN 8 //Minimum 8-byte ICMP packet (header)  
  
#define DEF_PACKET_SIZE 32  
#define MAX_PACKET 1024  
#define MAX_IP_HDR_SIZE 60  
  
//IP header structure  
typedef struct _iphdr  
{  
    unsigned int h_len:4;//Length of the header  
    unsigned int version:4;//Version of IP  
    unsigned char tos;//Type of service  
    unsigned short total_len;//Total length of the packet  
    unsigned short ident;//Unique identifier  
    unsigned short frag_and_flags;//Flags  
    unsigned char ttl;//Time to live  
    unsigned char proto;//Protocol (TCP,UDP,etc.)  
    unsigned short checksum;//IP checksum  
  
    unsigned int sourceIP;  
    unsigned int destIP;  
} IpHeader;  
  
//ICMP header structure  
typedef struct _icmphdr  
{  
    BYTE i_type;  
    BYTE i_code;//Type sub code  
    USHORT i_cksum;  
    USHORT i_id;  
    USHORT i_seq;  
  
    //This is not the standard header, but we reserve space for time  
    ULONG timestamp;  
} IcmpHeader;  
  
//IP option header--use with socket option IP_OPTIONS  
typedef struct _ipoptionhdr  
{  
    unsigned char code;//Option type  
    unsigned char len;//Length of option hdr  
    unsigned char ptr;//Offset into optons  
    unsigned long addr[9];//List of IP addrs  
} IpOptionHeader;  
  
int datasize;  
char* lpdest;  
  
//Print usage information  
void usage()  
{  
    printf("usage:MyPing -i:IP [data size]\n");  
    printf("    -i:IP           remote machine to Ping\n");  
    printf("    datasize     can be up to 1 KB\n");  
  
    ExitProcess(1);  
}  
  
//Helper function to fill in various fields for our ICMP request  
void FillICMPData(char* icmp_data, int datasize)  
{  
    IcmpHeader* icmp_hdr = (IcmpHeader*)icmp_data;  
    icmp_hdr->i_type = ICMP_ECHO;//Request an ICMP echo  
    icmp_hdr->i_code = 0;  
    icmp_hdr->i_id = (USHORT)GetCurrentProcessId();  
    icmp_hdr->i_cksum = 0;  
    icmp_hdr->i_seq = 0;  
  
    char* datapart = icmp_data + sizeof(IcmpHeader);  
  
    //Place some junk in the buffer  
    memset(datapart, 'E', datasize - sizeof(IcmpHeader));  
}  
  
//This function calculates the 16-bit one's complement sum  
//of the supplied buffer (ICMP) header  
USHORT checksum(USHORT* buffer, int size)  
{  
    unsigned long cksum = 0;  
  
    while (size > 1)  
    {  
        cksum += *buffer++;  
        size -= sizeof(USHORT);  
    }  
  
    if (size)  
    {  
        cksum += *(UCHAR*)buffer;  
    }  
  
    cksum = (cksum>>16) + (cksum & 0xffff);  
    cksum += (cksum>>16);  
  
    return (USHORT)(~cksum);  
}  
  
//If the IP option header is present, find the IP options  
//within the IP header and print the record route option values  
void DecodeIPOptions(char* buf, int bytes)  
{  
    IpOptionHeader* ipopt = (IpOptionHeader*)(buf + 20);  
  
    printf("RR:    ");  
    for (int i = 0; i < (ipopt->ptr / 4) - 1; i++)  
    {  
        IN_ADDR inaddr;  
        inaddr.S_un.S_addr = ipopt->addr[i];  
  
        if (i != 0)  
        {  
            printf("        ");  
        }  
  
        HOSTENT* host = gethostbyaddr((char*)&inaddr.S_un.S_addr,  
            sizeof(inaddr.S_un.S_addr), AF_INET);  
        if (host)  
        {  
            printf("(%-15s) %s\n", inet_ntoa(inaddr), host->h_name);  
        }  
        else  
        {  
            printf("(%-15s)\n", inet_ntoa(inaddr));  
        }  
    }  
  
    return;  
}  
  
//The response is an IP packet. We must decode the IP header to  
//locate the ICMP data.  
void DecodeICMPHeader(char* buf, int bytes, struct sockaddr_in* from)  
{  
    static int icmpcount = 0;  
    IpHeader* iphdr = (IpHeader*)buf;  
  
    //Number of 32-bit words * 4 = bytes  
    unsigned short iphdrlen = iphdr->h_len * 4;  
    DWORD tick = GetTickCount();  
  
    if ((iphdrlen == MAX_IP_HDR_SIZE) && (!icmpcount))  
    {  
        DecodeIPOptions(buf, bytes);  
    }  
  
    if (bytes < iphdrlen + ICMP_MIN)  
    {  
        printf("Too few bytes from %s\n", inet_ntoa(from->sin_addr));  
    }  
  
    IcmpHeader* icmphdr = (IcmpHeader*)(buf + iphdrlen);  
    if (icmphdr->i_type != ICMP_ECHOREPLY)  
    {  
        printf("nonecho type %d recvd\n", icmphdr->i_type);  
        return;  
    }  
  
    //Make sure this is an ICMP reply to something we sent!  
    if (icmphdr->i_id != (USHORT)GetCurrentProcessId())  
    {  
        printf("someone else's packet!\n");  
        return;  
    }  
  
    printf("%d bytes from %s:", bytes, inet_ntoa(from->sin_addr));  
    printf("  icmp_seq = %d. ", icmphdr->i_seq);  
    printf("  time:%d ms", tick - icmphdr->timestamp);  
    printf("\n");  
  
    icmpcount++;  
    return;  
}  
  
void ValidateArgs(int argc, char ** argv)  
{  
    lpdest = NULL;  
    datasize = DEF_PACKET_SIZE;  
  
    for (int i = 1; i < argc; i++)  
    {  
        if ((argv[i][0] == '-') || (argv[i][0] == '/'))  
        {  
            switch (tolower(argv[i][1]))  
            {  
            case 'i':  
                lpdest = argv[i] + 3;  
                break;  
            default:  
                usage();  
                break;  
            }  
        }  
        else if (isdigit(argv[i][0]))  
        {  
            datasize = atoi(argv[i]);  
        }  
    }  
}  
  
int main(int argc, char * argv[])  
{  
    WSADATA wsaData;  
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)  
    {  
        printf("WSAStartup() failed:%d\n", GetLastError());  
        return -1;  
    }  
  
    ValidateArgs(argc, argv);  
  
    SOCKET sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP,  
        NULL, 0, WSA_FLAG_OVERLAPPED);  
    if (sockRaw == INVALID_SOCKET)  
    {  
        printf("WSASocket() failed:%d\n", WSAGetLastError());  
        return -1;  
    }  
  
    //Set the send/recv timeout values  
    struct sockaddr_in from;  
    int fromlen = sizeof(from);  
    int timeout = 1000;  
  
    int bread = setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO,  
        (char*)&timeout, sizeof(timeout));  
    if (bread == SOCKET_ERROR)  
    {  
        printf("setsockopt(SO_RCVTIMEO) failed:%d\n", WSAGetLastError());  
        return -1;  
    }  
  
    timeout = 1000;  
    bread = setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO,  
        (char*)&timeout, sizeof(timeout));  
    if (bread == SOCKET_ERROR)  
    {  
        printf("setsockopt(SO_SNDTIMEO) failed:%d\n", WSAGetLastError());  
        return -1;  
    }  
  
    struct sockaddr_in dest;  
    memset(&dest, 0, sizeof(dest));  
  
    //Resolve the endpoint's name if necessary  
    dest.sin_family = AF_INET;  
  
    if ((lpdest != NULL) && strlen(lpdest) != 0)  
    {  
        dest.sin_addr.s_addr = inet_addr(lpdest);  
    }  
    else  
    {  
        struct hostent* hp = gethostbyname(lpdest);  
        if (hp != NULL)  
        {  
            memcpy(&(dest.sin_addr), hp->h_addr, hp->h_length);  
            dest.sin_family = hp->h_addrtype;  
            printf("dest.sin_addr = %s\n", inet_ntoa(dest.sin_addr));  
        }  
        else  
        {  
            printf("gethostbyname() failed:%d\n", WSAGetLastError());  
            return -1;  
        }  
    }  
  
    //Create the ICMP packet  
    datasize += sizeof(IcmpHeader);  
  
    char* icmp_data = (char*)HeapAlloc(GetProcessHeap(),  
        HEAP_ZERO_MEMORY, MAX_PACKET);  
    if (!icmp_data)  
    {  
        printf("HeapAlloc() failed:%d\n", GetLastError());  
        return -1;  
    }  
    memset(icmp_data, 0, MAX_PACKET);  
    FillICMPData(icmp_data, datasize);  
  
    char* recvbuf = (char*)HeapAlloc(GetProcessHeap(),  
        HEAP_ZERO_MEMORY, MAX_PACKET);  
  
    //Start sending/receiving ICMP packets  
    USHORT seq_no = 0;  
    while (true)  
    {  
        static int nCount = 0;  
        int bwrote;  
  
        if (nCount++ == 4)  
        {  
            break;  
        }  
  
        ((IcmpHeader*)icmp_data)->i_cksum = 0;  
        ((IcmpHeader*)icmp_data)->timestamp = GetTickCount();  
        ((IcmpHeader*)icmp_data)->i_seq = seq_no++;  
        ((IcmpHeader*)icmp_data)->i_cksum =  
            checksum((USHORT*)icmp_data, datasize);  
  
        bwrote = sendto(sockRaw, icmp_data, datasize, 0,  
            (struct sockaddr*)&dest, sizeof(dest));  
        if (bwrote == SOCKET_ERROR)  
        {  
            if (WSAGetLastError() == WSAETIMEDOUT)  
            {  
                printf("timed out\n");  
                continue;  
            }  
  
            printf("sendto() failed:%d\n", WSAGetLastError());  
            return -1;  
        }  
  
        if (bwrote < datasize)  
        {  
            printf("Wrote %d bytes\n", bwrote);  
        }  
  
        bread = recvfrom(sockRaw, recvbuf, MAX_PACKET, 0,  
            (struct sockaddr*)&from, &fromlen);  
        if (bread == SOCKET_ERROR)  
        {  
            if (WSAGetLastError() == WSAETIMEDOUT)  
            {  
                printf("timed out\n");  
                continue;  
            }  
  
            printf("revefrom() failed:%d\n", WSAGetLastError());  
            return -1;  
        }  
  
        DecodeICMPHeader(recvbuf, bread, &from);  
  
        Sleep(1000);  
    }  
  
    //Cleanup  
    if (sockRaw != INVALID_SOCKET)  
    {  
        closesocket(sockRaw);  
    }  
  
    HeapFree(GetProcessHeap(), 0, recvbuf);  
    HeapFree(GetProcessHeap(), 0, icmp_data);  
  
    WSACleanup();  
  
    return 0;  
}  