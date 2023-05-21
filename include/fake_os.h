#include "fake_process.h"
#include "linked_list.h"
#include "fake_cpu.h"
#pragma once 

typedef struct{
    ListItem list;
    int pid;
    ListHead events;
}FakePCB;

struct FakeOS;
typedef void(*ScheduleFn)(struct FakeOS* os, void* args);

typedef struct FakeOS{
    ListHead running; 
    ListHead ready;
    ListHead waiting;
    int timer;
    ScheduleFn schedule_fn;
    void* schedule_args;
    int tot_num_cpu;
    int busy_cpu;
    ListHead processes;
}FakeOS;

void FakeOS_init(FakeOS* os, int num_cpu);
void FakeOS_simStep(FakeOS* os);
