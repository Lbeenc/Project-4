// worker.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include "shared.h"

int main() {
    srand(getpid());
    int msgID = msgget(MSG_KEY, 0666);
    Message msg;

    if (msgrcv(msgID, &msg, sizeof(Message) - sizeof(long), getpid(), 0) == -1) {
        perror("worker msgrcv");
        exit(1);
    }

    int used = rand() % msg.usedTime + 1;
    msg.mtype = getppid();
    msg.usedTime = used;
    msg.willTerminate = (rand() % 5 == 0);

    if (msgsnd(msgID, &msg, sizeof(Message) - sizeof(long), 0) == -1) {
        perror("worker msgsnd");
        exit(1);
    }
    return 0;
}
