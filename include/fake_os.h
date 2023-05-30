#include "fake_process.h"
#include "linked_list.h"
#pragma once 

typedef struct{
    ListItem list;
    int pid;
    ListHead events;
    int start_time;
    int duration_time;
    float prediction_time;
    int update;
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
    ListHead processes;
}FakeOS;

//Inizialized a FakeOS
void FakeOS_init(FakeOS* os, int num_cpu);
//Implement one step of simulation
void FakeOS_simStep(FakeOS* os);
//Destroy
void FakeOS_destroy(FakeOS* os);
