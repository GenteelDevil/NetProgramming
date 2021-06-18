#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    int client_sockfd;
    assert(listenfd >= 0);
    ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);
    ret = listen(listenfd, 5);
    assert(ret != -1);



    struct sockaddr_in client_address; 

    char buf[1024];
    fd_set read_fds;
    fd_set exception_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&exception_fds);

    FD_SET(listenfd, &read_fds);

    while (1) {

        ret = select(FD_SETSIZE, &read_fds, NULL, &exception_fds, NULL);
        if (ret <= 0) {
            printf("selection failure\n");
        }
        for (int fd = 0; fd < FD_SETSIZE; ++fd) {
            // 遍历比较相关的文件描述符
            if (FD_ISSET(fd, &read_fds)) {
                // 判断是否为服务器套接字，如果是则表明为客户连接请求
                if (fd == listenfd) {
                    socklen_t clien_addrlength = sizeof(client_address);
                    client_sockfd = accept(listenfd, (struct sockaddr*)&client_address, &clien_addrlength);
                    FD_SET(client_sockfd, &read_fds);
                    printf("new connected client: %d\n", client_sockfd);
                }
            }
        }

        printf("%d sockfd to process\n", ret);
        if (FD_ISSET(connfd, &read_fds)) {
            ret = recv(connfd, buf, sizeof(buf) - 1, 0);
            if (ret <= 0) {
                printf("normal failed\n");
                break;
            }
            printf("get %d bytes of normal data: %s\n", ret, buf);
        } else if (FD_ISSET(connfd, &exception_fds)) {
            ret = recv(connfd, buf, sizeof(buf) - 1, MSG_OOB);
            if (ret <= 0) {
                printf("oob failed\n");
                break;
            } 
                
            printf("get %d bytes of oob data: %s\n", ret, buf);
        }

    }
    close(connfd);
    close(listenfd);
    return 0;
}