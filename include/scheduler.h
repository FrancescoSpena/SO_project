#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "fake_os.h"
#pragma once

typedef struct{
    int next_quantum;
    int curr_quantum;
    int pred_quantum;
    float alpha;
} SchedSJFArgs;

//Update value of quantum
void updateQuantum(SchedSJFArgs *a);
//Return the process with min burst
FakePCB *minBurst(ListHead *ready);
//Update all event of running queue
void updateEvent(ListHead *cpu);
//Change a process max burst with min
void changeProcess(FakeOS* os, FakePCB* min);
// Routine free cpu
void choiceProcessFreeCPU(FakeOS *os);
// Routine busy cpu
void choiceProcessBusyCPU(FakeOS *os, int curr_quantum);
//Scheduler SJF preemptive
void schedSJF(FakeOS *os, void *args_);