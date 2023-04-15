
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/poll.h>

#define BUFFER_LENGTH 1024

#define POLL_SIZE 1024

// 线程执行函数
void *client_thread(void *arg) {
    int clientFd = *(int*)arg;

    while (1)
    {
        char buffer[BUFFER_LENGTH] = {0};
        int ret = recv(clientFd, buffer, BUFFER_LENGTH, 0);
        if (ret == 0) {
            // 客户端断开处理
            close(clientFd);
            break;
        }

        printf("ret: %d, buffer: %s\n", ret, buffer);

        send(clientFd, buffer, ret, 0);
    }
}

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

    // sleep(10);

#if 0
    // 设置socketFd为非阻塞
    int flags = fcntl(socketFd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(socketFd, F_SETFL, flags);
#endif

    //为客户端建立新连接
    struct sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);

#if 0

    while (1) {
        int clientFd = accept(socketFd, (struct sockaddr *)&clientAddr, &len); // block: 等待一个客户端连接

        // printf("clientFd: %d, socketFd: %d\n", clientFd, socketFd);

#if 0
    char buffer[BUFFER_LENGTH] = {0};
    int ret = recv(clientFd, buffer, BUFFER_LENGTH, 0);

    printf("ret: %d, buffer: %s\n", ret, buffer);

    send(clientFd, buffer, ret, 0);
#else

        pthread_t threadId;
        pthread_create(&threadId, NULL, client_thread, &clientFd);
    }
#endif

#elif 0 //select

    // select(maxfd, rfds, wfds, efds, timeout);
    fd_set rfds, rset;
    FD_ZERO(&rfds);
    FD_SET(socketFd, &rfds);

    int maxfd = socketFd;
    int clientFd = 0;

    while (1) {
        rset = rfds;

        int nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

        printf("select event\n");

        //检测bit图的下标为socketFd是否为1
        if (FD_ISSET(socketFd, &rset)) {
            clientFd = accept(socketFd, (struct sockaddr *)&clientAddr, &len);
            printf("accept: %d\n", clientFd);

            FD_SET(clientFd, &rfds);
            if (clientFd > maxfd) maxfd = clientFd;

            if (-- nready == 0) continue;
        }

        int i = 0;
        for (i = socketFd+1; i <= maxfd; i++) {
            if (FD_ISSET(i, &rset)) {
                char buffer[BUFFER_LENGTH] = {0};
                int ret = recv(clientFd, buffer, BUFFER_LENGTH, 0);
                if (ret == 0)
                {
                    // 客户端断开处理
                    close(clientFd);
                    break;
                }

                printf("ret: %d, buffer: %s\n", ret, buffer);

                send(clientFd, buffer, ret, 0);
            }
        }
    }
#else

    struct pollfd fds[POLL_SIZE] = {0};

    fds[socketFd].fd = socketFd;
    fds[socketFd].events = POLLIN; // events: 需要监听的事件

    int maxfd = socketFd;
    int clientFd = 0;

    while (1) {
        int nready = poll(fds, maxfd+1, -1);

        // revents: 就绪事件
        // 监听fd就绪
        if (fds[socketFd].revents & POLLIN) {
            clientFd = accept(socketFd, (struct sockaddr *)&clientAddr, &len);
            printf("accept: %d\n", clientFd);

            fds[clientFd].fd = clientFd;
            fds[clientFd].events = POLLIN;

            if (clientFd > maxfd) maxfd = clientFd;

            if (-- nready == 0) continue;
        }

        // 其他fd就绪
        int i = 0;
        for (i = socketFd+1; i <= maxfd; i++) {
            if (fds[i].revents & POLLIN) {
                char buffer[BUFFER_LENGTH] = {0};
                int ret = recv(i, buffer, BUFFER_LENGTH, 0);
                if (ret == 0)
                {
                    // 客户端断开处理
                    fds[i].fd = -1;
                    fds[i].events = 0;
                    close(i);
                    break;
                }

                printf("ret: %d, buffer: %s\n", ret, buffer);

                send(i, buffer, ret, 0);
            }
        }
    }

#endif

}
