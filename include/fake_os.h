#include "fake_process.h"
#include "linked_list.h"
#pragma once 

typedef struct{
    ListItem list;
    int pid;
    ListHead events;
}FakePCB;

typedef struct{
    FakePCB* execution;
    int time_exection;
}FakeCPU;

struct FakeOS;
typedef void(*ScheduleFn)(struct FakeOS* os, void* args);

typedef struct FakeOS{
    ListHead cpu;
    ListHead running; 
    ListHead ready;
    ListHead waiting;
    int timer;
    ScheduleFn schedule_fn;
    void* schedule_args;
    int tot_num_cpu;
    int busy_cpu;
    int *time_process_execution;
    ListHead processes;
}FakeOS;

void FakeOS_init(FakeOS* os, int num_cpu);
void FakeOS_simStep(FakeOS* os);
