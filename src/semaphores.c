#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>

union semun {
    int val;
    struct semid_ds* buf;
    unsigned short int* array;
    struct seminfo* __buf;
};

// op为-1的时候执行P操作，op为1的时候执行V操作
void pv(int sem_id, int op) {
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = op;
    sem_b.sem_flg = SEM_UNDO;
    semop(sem_id, &sem_b, 1);
}

int main() {
    // IPC_PRIVETE是一个特殊的键值，无论信号量集是否存在，都新建一个
    int sem_id = semget(IPC_PRIVATE, 1, 0666);
    union semnu sem_un;
    // 设置信号量初始化为1，实际上就是调用semctl把用户态的数据复制到内核态数据中
    sem_un.val = 1;
    semctl(sem_id, 0, SETVAL, sem_un);
    pid_t id = fork();
    // fork出错
    if (id < 0) {
        return 1;
    } else if (id == 0) {  // 在子进程中
        printf("in child process\n");
        pv(sem_id, -1);
        printf("child process get the sem and would release it after 3 seconds\n");
        sleep(3);
        pv(sem_id, 1);
        exit(0);
    } else {
        printf("in father process\n");
        pv(sem_id, -1);
        printf("father process get the sem and would release it after 3 seconds\n");
        pv(sem_id, 1);
    }
    waitpid(id, NULL, 0);
    semctl(sem_id, 0, IPC_RMID, sem_un);
    return 0;
}
