#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include <stdlib.h>

#define SRVADDR "127.0.0.1"

// 服务端
int main() {
    struct sockaddr_in seraddr, cliaddr;
    memset(&seraddr, 0, sizeof(struct sockaddr_in));
    char buf[1024];
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(8889);
    seraddr.sin_addr.s_addr = INADDR_ANY;


    int serfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(serfd < 0) {
        perror("socket");
        exit(1);
    }
    if(bind(serfd, (struct sockaddr*)&seraddr, sizeof(seraddr)) == -1) {
        perror("bind");
        exit(1);
    }
    
    while (1)
    {
        int clilen = sizeof(cliaddr);
        int len = recvfrom(serfd, buf, sizeof(buf), 0, (struct sockaddr*)&cliaddr, &clilen);
        
        char infofrom[INET_ADDRSTRLEN] = {0};
        printf("cliip: %s, cliport: %d, len: %d , recvfrom-> %s",
                inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, infofrom, sizeof(infofrom)), ntohs(cliaddr.sin_port),len,buf);

        sleep(5);			// 延时5s回声，增加服务器处理时常，以更清楚看到udp是串行处理
        sendto(serfd, buf, sizeof(buf), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));

    }
    close(serfd);
    return 0;
}
