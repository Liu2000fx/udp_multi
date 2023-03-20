#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<pthread.h>
#include <string.h>
 
#define SERVER_PORT 8889
#define BUFF_LEN 512
#define SERVER_IP "127.0.0.1"
void* timer1(void* a){
    printf("pid: %ld\n", pthread_self());
    int ti = 0;
    while(1) {
        clock_t delay= CLOCKS_PER_SEC;
        clock_t start=clock();
        while(clock()-start < delay);
        ti++;
        pthread_testcancel();
        printf("time: %d\n", ti);

    }
}
 
 
void udp_msg_sender(int fd, struct sockaddr* dst)
{
 
    socklen_t len;
    struct sockaddr_in src;
    while(1)
    {
        char buf[BUFF_LEN] = "TEST UDP MSG!\n";
        len = sizeof(*dst);
        printf("client:%s\n",buf);       //打印自己发送的信息
        sendto(fd, buf, BUFF_LEN, 0, dst, len);
        memset(buf, 0, BUFF_LEN);

        pthread_t pid;
        pthread_create(&pid, NULL, timer1, NULL);		// 计时打印

        recvfrom(fd, buf, BUFF_LEN, 0, (struct sockaddr*)&src, &len);  //接收来自server的信息
        pthread_cancel(pid);
        printf("server:%s\n",buf);
        // sleep(1);  //一秒发送一次消息
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
 
    udp_msg_sender(client_fd, (struct sockaddr*)&ser_addr);
    close(client_fd);
 
    return 0;
}
