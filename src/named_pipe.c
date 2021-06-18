#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FIFO "./testfifo"

int main() {
    int fd, fd_f;
    pid_t id = fork();
    if (id == 0) {
        char *msg = "I am a child\n";
        fd = open(FIFO, O_WRONLY);
        int nwrite = write(fd, msg, strlen(msg) + 1);
        printf("send %d\n", nwrite);
        close(fd);
    } else if (id > 0) {
        char msgbuff[100];
        if (access(FIFO, F_OK) == -1) {
            if (mkfifo(FIFO, O_CREAT | O_EXCL) < 0 && errno != EEXIST) {
                printf("father can not fifo\n");
                return 1;
            }
        }
        memset(msgbuff, '\0', sizeof(msgbuff));
        fd_f = open(FIFO, O_RDONLY);
        if (fd_f == -1) {
            printf("father open errno\n");
            return 1;
        }
        int nread = read(fd_f, msgbuff, sizeof(msgbuff));
        if (nread > 0) {
            printf("father read %s \n", msgbuff);
        }
        close(fd_f);
    }
    return 0;
}