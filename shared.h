// shared.h
#ifndef SHARED_H
#define SHARED_H

#include <sys/types.h>

#define MAX_PROCESSES 18
#define MSG_KEY 0x1234
#define SHM_KEY 0x5678
#define QUANTUM_BASE 10000000
#define MAX_TOTAL_PROCS 40
#define NUM_QUEUES 3

// Simulated Clock
typedef struct {
    unsigned int seconds;
    unsigned int nanoseconds;
} SimClock;

// Process Control Block
typedef struct {
    int occupied;
    pid_t pid;
    int startSeconds;
    int startNano;
    int totalCpuTime;
    int lastBurst;
    int queueLevel;
    int blocked;
} PCB;

// Message structure for IPC
typedef struct {
    long mtype;
    int usedTime;
    int willTerminate;
} Message;

#endif // SHARED_H
