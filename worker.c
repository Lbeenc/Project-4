// worker.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <time.h>

#define MSG_KEY 0x1234
#define MAX_PROCESSES 18

typedef struct {
    long mtype;
    int usedTime;
    int willTerminate;
} Message;

int main() {
    srand(getpid());
    int msgID = msgget(MSG_KEY, 0666);

    Message msg;
    msgrcv(msgID, &msg, sizeof(Message) - sizeof(long), getpid(), 0);

    int used = rand() % msg.usedTime;
    msg.mtype = getppid();
    msg.usedTime = used;
    msg.willTerminate = rand() % 5 == 0 ? 1 : 0;

    msgsnd(msgID, &msg, sizeof(Message) - sizeof(long), 0);
    return 0;
}
