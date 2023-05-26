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

//Scheduler SJF preemptive
void schedSJF(FakeOS *os, void *args_);