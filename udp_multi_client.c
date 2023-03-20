#include <stdio.h>
#include<arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<pthread.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>
 
#define SERVER_PORT 8889
#define BUFF_LEN 512
#define SERVER_IP "127.0.0.1"
void* timer1(void* a){
    printf("pid: %ld\n", pthread_self());
    int ti = 0;
    while(1) {
        clock_t delay= CLOCKS_PER_SEC;          // 计时1s，线程不死一直计时
        clock_t start=clock();
        while(clock()-start < delay);
        ti++;
        pthread_testcancel();
        printf("time: %d\n", ti);

    }
}

// 接收案例，并发udp执行这个
void udp_msg_recv(int fd, struct sockaddr* dst)     
{
    socklen_t len;
    struct sockaddr_in src;

    char buf[BUFF_LEN] = "TEST UDP MSG!\n";     // 测试包，为了告诉服务器我要连接了
    len = sizeof(*dst);
    printf("client connect request:  %s\n",buf);  // 打印自己发送的信息
    sendto(fd, buf, BUFF_LEN, 0, dst, len);         // 发送测试包的信息
    memset(buf, 0, BUFF_LEN);
    printf("Wait for the server to connect!\n");  
    recvfrom(fd, buf, BUFF_LEN, 0, (struct sockaddr*)&src, &len);  // 接收来自server的信息

    char srvip[INET_ADDRSTRLEN] = {0};
    printf("new client connect addr: ip-> %s, port-> %d, info --> %s\n", inet_ntop(AF_INET, &src.sin_addr.s_addr, srvip, sizeof(srvip)), ntohs(src.sin_port),buf); 
    struct sockaddr_in* newsrv = (struct sockaddr_in *)dst;
    newsrv->sin_port = src.sin_port;        // 更换新的port

    // 循环作业
    while(1)
    {
        char buf[BUFF_LEN] = "UDP multi!\n";
        len = sizeof(*newsrv);

        printf("send addr ip-> %s, port-> %d, send info ->%s\n", inet_ntop(AF_INET, &newsrv->sin_addr.s_addr, srvip, sizeof(srvip)), ntohs(newsrv->sin_port), buf); 
        sendto(fd, buf, BUFF_LEN, 0, (struct sockaddr*)newsrv, len);
        memset(buf, 0, BUFF_LEN);

        pthread_t pid;
        pthread_create(&pid, NULL, timer1, NULL);

        recvfrom(fd, buf, BUFF_LEN, 0, (struct sockaddr*)newsrv, &len);  //接收来自server的信息
        
        printf("server:%s\n",buf);
        pthread_cancel(pid);
    }
}
 
int main(int argc, char* argv[])
{
    int client_fd;
    struct sockaddr_in ser_addr;
 
    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(client_fd < 0)
    {
        printf("create socket fail!\n");
        return -1;
    }
 
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    //ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //注意网络序转换
    ser_addr.sin_port = htons(SERVER_PORT);  //注意网络序转换
 
    udp_msg_recv(client_fd, (struct sockaddr*)&ser_addr);
 
    close(client_fd);
 
    return 0;
}
