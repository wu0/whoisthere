/* Auth:9crk mail:admin@9crk.com */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <linux/sockios.h>
#include <ifaddrs.h>

#define PASSWORD "9crk"
#define LISTEN_PORT	888

int im_here(int port, const char* dev_name_like_eth0)
{
	int socketfd;
    socklen_t addr_len;
    char buf[64];
    struct sockaddr_in server_addr;
    struct sockaddr_in *sin;
    struct ifreq ifr;
	pthread_t wait_thread, tmp_thread;
	
//open handle
    if((socketfd = socket(AF_INET,SOCK_DGRAM,0)) < 0){
        perror("socket");
        return -1;
    }
//get broadcast IP
	strcpy(ifr.ifr_name, dev_name_like_eth0);
    if(ioctl(socketfd, SIOCGIFBRDADDR/*SIOCGIFADDR*/ ,&ifr) < 0){
        perror("ioctl error\n");
        return -1;
    }
    sin = (struct sockaddr_in *)&(ifr.ifr_addr);
//get own IP
/*    if(ioctl(socketfd, SIOCGIFADDR, &ifr) < 0){
        perror("ioctl error\n");
        return -1;
    }*/
//initial recving
	memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr = sin->sin_addr;
    server_addr.sin_port = htons(port);
    addr_len=sizeof(server_addr);
	
	if(bind(socketfd,(struct sockaddr*)&server_addr,addr_len) < 0){
        perror("bind");
        exit(-1);
    }

//forever keep recv and echo
	while(1){
		if(recvfrom(socketfd,buf,64,0,(struct sockaddr*)&server_addr,&addr_len) < 0){
            perror("recvErr");
            return -1;
        }else{
			if(strcmp(buf, PASSWORD) == 0){	 
				sendto(socketfd,"echo", 4, 0, (struct sockaddr *)&server_addr,addr_len);
				printf("told once!\n");
			}
		}
	}
}
int main(int argc, char*argv[])
{
	if(argc != 2){
		printf("sample usage:%s eth0\n", argv[0]);
	}
	im_here(LISTEN_PORT, argv[1]);
}

