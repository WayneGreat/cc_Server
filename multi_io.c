
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>


int main() {
    //创建服务器fd
    //@AF_INET ipv4协议
    //@SOCK_SREAM 字节流
    int socketFd = socket(AF_INET, SOCK_STREAM, 0); //io

    //设置需监听的地址、端口
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //0.0.0.0
    serverAddr.sin_port = htons(9999);

    //绑定地址端口到fd
    if (-1 == bind(socketFd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in))) {
        printf("bind failed: %s", strerror(errno));
        return -1;
    }

    //监听地址端口
    listen(socketFd, 10);

#if 1

#if 1


#endif

    //为客户端建立新连接
    struct sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    int clientFd = accept(socketFd, (struct sockaddr *)&clientAddr, &len); // block: 等待一个客户端连接
#endif

    getchar(); // block

    getchar();

};