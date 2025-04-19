// oss.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include "shared.h"

PCB *pcbTable;
SimClock *simClock;
int shmClockID, msgID, shmPCBID;
FILE *logFile;
int totalProcs = 0;
int activeProcs = 0;

int queues[NUM_QUEUES][MAX_PROCESSES];
int queueSize[NUM_QUEUES] = {0};

void enqueue(int level, int pidIndex) {
    queues[level][queueSize[level]++] = pidIndex;
}

int dequeue(int level) {
    if (queueSize[level] == 0) return -1;
    int pidIndex = queues[level][0];
    for (int i = 1; i < queueSize[level]; i++)
        queues[level][i - 1] = queues[level][i];
    queueSize[level]--;
    return pidIndex;
}

void cleanup(int sig) {
    msgctl(msgID, IPC_RMID, NULL);
    shmdt(simClock);
    shmctl(shmClockID, IPC_RMID, NULL);
    shmdt(pcbTable);
    shmctl(shmPCBID, IPC_RMID, NULL);
    fclose(logFile);
    printf("\n[oss] Cleaned up resources. Exiting.\n");
    exit(0);
}

void incrementClock(unsigned int ns) {
    simClock->nanoseconds += ns;
    while (simClock->nanoseconds >= 1000000000) {
        simClock->seconds++;
        simClock->nanoseconds -= 1000000000;
    }
}

void forkWorker(int index) {
    pid_t pid = fork();
    if (pid == 0) {
        execl("./worker", "worker", NULL);
        perror("execl");
        exit(1);
    }
    pcbTable[index].pid = pid;
    pcbTable[index].startSeconds = simClock->seconds;
    pcbTable[index].startNano = simClock->nanoseconds;
    pcbTable[index].queueLevel = 0;
    pcbTable[index].occupied = 1;
    enqueue(0, index);
    totalProcs++;
    activeProcs++;
}

int main() {
    signal(SIGINT, cleanup);
    logFile = fopen("log.txt", "w");

    shmClockID = shmget(SHM_KEY, sizeof(SimClock), IPC_CREAT | 0666);
    simClock = (SimClock *)shmat(shmClockID, NULL, 0);
    simClock->seconds = 0;
    simClock->nanoseconds = 0;

    shmPCBID = shmget(SHM_KEY + 1, sizeof(PCB) * MAX_PROCESSES, IPC_CREAT | 0666);
    pcbTable = (PCB *)shmat(shmPCBID, NULL, 0);
    memset(pcbTable, 0, sizeof(PCB) * MAX_PROCESSES);

    msgID = msgget(MSG_KEY, IPC_CREAT | 0666);

    while (totalProcs < MAX_TOTAL_PROCS || activeProcs > 0) {
        if (totalProcs < MAX_TOTAL_PROCS && activeProcs < MAX_PROCESSES) {
            for (int i = 0; i < MAX_PROCESSES; i++) {
                if (!pcbTable[i].occupied) {
                    forkWorker(i);
                    break;
                }
            }
        }

        int scheduled = 0;
        for (int q = 0; q < NUM_QUEUES && !scheduled; q++) {
            int pidIndex = dequeue(q);
            if (pidIndex == -1) continue;

            int quantum = QUANTUM_BASE * (1 << q);
            Message msg;
            msg.mtype = pcbTable[pidIndex].pid;
            msg.usedTime = quantum;
            msg.willTerminate = 0;
            msgsnd(msgID, &msg, sizeof(Message) - sizeof(long), 0);

            msgrcv(msgID, &msg, sizeof(Message) - sizeof(long), getpid(), 0);

            incrementClock(msg.usedTime);
            pcbTable[pidIndex].totalCpuTime += msg.usedTime;
            pcbTable[pidIndex].lastBurst = msg.usedTime;

            if (msg.willTerminate) {
                fprintf(logFile, "Terminated: PID %d at %u:%u\n", pcbTable[pidIndex].pid, simClock->seconds, simClock->nanoseconds);
                kill(pcbTable[pidIndex].pid, SIGTERM);
                waitpid(pcbTable[pidIndex].pid, NULL, 0);
                pcbTable[pidIndex].occupied = 0;
                activeProcs--;
            } else {
                int newQueue = q == NUM_QUEUES - 1 ? q : q + 1;
                enqueue(newQueue, pidIndex);
            }
            scheduled = 1;
        }
        incrementClock(1000);
    }

    cleanup(0);
    return 0;
}
