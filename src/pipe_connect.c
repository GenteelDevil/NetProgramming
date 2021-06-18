#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    int fd[2];
    int ret = pipe(fd);
    if (ret == -1) {
        perror("pipe error\n");
        return 1;
    }
    pid_t id = fork();
    // 如果在子进程中
    if (id == 0) {
        int pid = getpid();
        int i = 0;
        close(fd[0]);
        char *child = "I am a child";
        for (int i = 0; i < 5; ++i) {
            write(fd[1], child, strlen(child) + 1);
            sleep(2);
        }
    } else if (id > 0) {  // 在父进程中
        close(fd[1]);
        char msgbuf[100] = {'\0'};
        for (int i = 0; i < 5; ++i) {
            memset(msgbuf, '\0', sizeof(msgbuf));
            ssize_t s = read(fd[0], msgbuf, sizeof(msgbuf));
            if (s > 0) {
                printf("father process %d: receive data %s\n", id, msgbuf);
            }
        }
    } else {
        perror("fork erronr\n");
        return 2;
    }
    return 0;
}