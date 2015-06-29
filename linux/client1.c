
	#include <stdio.h> 
	#include <unistd.h>
	#include <strings.h>
	#include <sys/types.h> 
	#include <sys/socket.h> 
	#include <netinet/in.h> 
	#include <netdb.h>        /* netbd.h is needed for struct hostent  */ 

	#define PORT 2999   /* Open Port on Remote Host */ 
	#define MAXDATASIZE 100   /* Max number of bytes of data */ 

int main(int argc, char *argv[]) 
{ 
	int fd, numbytes;   /* files descriptors */ 
	char buf[MAXDATASIZE];  /* buf will store received text */ 
	struct hostent *he;         /* structure that will get information about remote host */ 
	struct sockaddr_in server;  /* server's address information */ 

	if (argc !=2) {       /* this is used because our program will need one argument (IP) */ 
	printf("Usage: %s <IP Address>\n",argv[0]); 
	return 1;  
	} 

	if ((he=gethostbyname(argv[1]))==NULL){ /* calls gethostbyname() */ 
	printf("gethostbyname() error\n"); 
	return 1; 
	} 

	if ((fd=socket(AF_INET, SOCK_STREAM, 0))==-1){  /* calls socket() */ 
	printf("socket() error\n"); 
	return 1; 
	} 

	bzero(&server,sizeof(server));
	server.sin_family = AF_INET; 
	server.sin_port = htons(PORT); /* htons() is needed again */ 
	server.sin_addr = *((struct in_addr *)he->h_addr);  /*he->h_addr passes "*he"'s info to "h_addr" */ 

	if(connect(fd, (struct sockaddr *)&server,sizeof(struct sockaddr))==-1){ /* calls connect() */ 
	printf("connect() error\n"); 
	return 1; 
	} 

	while(1)
	{
		if ((numbytes=recv(fd,buf,MAXDATASIZE,0)) == -1){  /* calls recv() */ 
		printf("recv() error\n"); 
		return 1; 
		} 

		buf[numbytes]='\0'; 
		printf("Server Message: %s\n",buf); /* it prints server's welcome message  */ 
	}
	close(fd);   /* close fd */ 
	return 1; 
}


