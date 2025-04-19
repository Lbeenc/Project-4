# CMP SCI 4760 - Project #4: Multi-Level Feedback Queue Scheduler

## Author
Curtis Been

## Description
This project simulates a Multi-Level Feedback Queue (MLFQ) process scheduler using:
- A simulated system clock
- Shared memory for PCB and clock data
- Message queues for process communication
- Three priority levels with aging and demotion logic

It consists of two main programs:
- `oss`: The main scheduler process
- `worker`: The user process that receives a quantum and simulates execution

## Files Included
- `oss.c`: Main scheduler
- `worker.c`: Simulated user process
- `shared.h`: Shared definitions (clock, PCB, message struct)
- `Makefile`: Compilation tool
- `log.txt`: Scheduler log output (generated)

## Compilation
To compile all necessary files:
```bash
make
