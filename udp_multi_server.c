#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include <stdlib.h>

int idx = 0;

#define SRVADDR "127.0.0.1"
void* connectcli(void* arg) {
    printf("pid  ->  %ld, arg addr-> %p\n", pthread_self(), arg);
    struct sockaddr_in* cliaddr = (struct sockaddr_in*)arg;
    struct sockaddr_in newaddr ;
    newaddr.sin_family = AF_INET;
    newaddr.sin_port = htons(9990 + idx);
    inet_pton(AF_INET, SRVADDR, &newaddr.sin_addr.s_addr);

    char cliip[INET_ADDRSTRLEN] = {0};
    printf("cliip-> %s, port-> %d\n", inet_ntop(AF_INET, &cliaddr->sin_addr.s_addr, cliip, sizeof(cliip)), ntohs(cliaddr->sin_port)); 

    int newfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(bind(newfd, (struct sockaddr*)&newaddr, sizeof(newaddr)) < 0) {
        perror("new bind");
        exit(1);
    }

    printf("new fd->%d,  ip-> %s, port-> %d\n",newfd, inet_ntop(AF_INET, &newaddr.sin_addr.s_addr, cliip, sizeof(cliip)), ntohs(newaddr.sin_port)); 
    
    connect(newfd, (struct sockaddr*)cliaddr, sizeof(*cliaddr));
    
    char con[] = "send new connect";
    sendto(newfd, con, sizeof(con),0, (struct sockaddr*)cliaddr, sizeof(*cliaddr));
    
    printf("over!!!\n");
    char cli[512] ;
    while(1) {
        struct sockaddr_in clia;
        int len = sizeof(clia);
        recvfrom(newfd, cli, sizeof(cli), 0, (struct sockaddr*)&clia, &len);  //接收来自cli的信息

        sleep(5);
        sendto(newfd, cli, sizeof(cli), 0, (struct sockaddr*)&clia, sizeof(clia));
        printf("send to ip-> %s, port-> %d\n", inet_ntop(AF_INET, &clia.sin_addr.s_addr, cliip, sizeof(cliip)), ntohs(clia.sin_port)); 
    }
    return NULL;
}

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
        printf("client first connect: ip %s, port: %d, len: %d , recvfrom-> %s",
                inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, infofrom, sizeof(infofrom)), ntohs(cliaddr.sin_port), len, buf);

        pthread_t pid;
        idx ++;
        printf("===============\n");

        struct sockaddr_in cliaddr2  ;
        cliaddr2.sin_family = AF_INET;
        cliaddr2.sin_addr.s_addr = cliaddr.sin_addr.s_addr;
        cliaddr2.sin_port = cliaddr.sin_port;

        pthread_create(&pid, NULL, connectcli, (void*)&cliaddr2);
        pthread_detach(pid);
        printf("===============\n");

    }
    
    close(serfd);

    return 0;
}
