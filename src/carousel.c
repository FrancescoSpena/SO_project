#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../include/fake_os.h"

FakeOS os;

typedef struct{
    int next_quantum;
    int curr_quantum;
    int pred_quantum;
    float alpha;
}SchedSJFArgs;

void update_quantum(SchedSJFArgs* args){
    args->next_quantum = args->alpha*args->curr_quantum + (1 - args->alpha) * args->pred_quantum;
}

void schedSJF(FakeOS *os, void* args_){
    SchedSJFArgs *args = (SchedSJFArgs*)args_;

    // look for the first process in ready
    // if none, return
    if(!os->ready.first){
        return;
    }

    //TODO take a min cpu burst NOT the first
    FakePCB* pcb = (FakePCB*)List_popFront(&os->ready);
   
    //check if all cpu are busy
    //if the number of busy < total num of cpu
    if(os->cpu.size < os->num_cpu){
        printf("some free cpu\n");
        List_pushBack(&os->running,(ListItem*)pcb);
        FakeCPU* new_cpu = (FakeCPU*)malloc(sizeof(FakeCPU));
        List_pushBack(&os->cpu,(ListItem*)new_cpu);
        new_cpu->status=RUNNING;
    }
    //_______________
        //Add process to ready list to running 
        //in cpu, set the status = running and 
        //add to running list
    //_______________
        /*
        For each cpu check the status and if the 
        cpu burst of process in running is > 
        new select process cpu burst change else 
        do nothing.
        */
    //_______________

}

int main(int argc, char** argv){
    
    if(argc <= 1){
        printf("usage: %s <num_core> <processes>", argv[0]);
        return 0;
    }
    
    printf("Go the carousel\n");
    int num_cpu = atoi(argv[1]);
    FakeOS_init(&os, num_cpu);

    //scheduler args
    SchedSJFArgs srr_args;
    srr_args.curr_quantum = 2;
    srr_args.alpha = 0.3;
    srr_args.next_quantum = 0;
    srr_args.pred_quantum = 0;
    os.schedule_fn=schedSJF;
    os.schedule_args=&srr_args;

    //argv[1] = num_core
    for(int i=2; i < argc; i++){
        FakeProcess new_process;
        int num_events = FakeProcess_load(&new_process,argv[i]);
        printf("loading [%s], pid: %d, events:%d",
           argv[i], new_process.pid, num_events);
        
        if(num_events){
            FakeProcess* new_process_ptr=(FakeProcess*)malloc(sizeof(FakeProcess));
            *new_process_ptr = new_process;
            List_pushBack(&os.processes,(ListItem*)new_process_ptr);
        }
    }
    printf("\nnum core selected %d\n", os.num_cpu);
    printf("num processes in queue %d\n", os.processes.size);

    /*while(os.running.first
        || os.ready.first
        || os.waiting.first
        || os.processes.first){
        FakeOS_simStep(&os);
    }*/

    return 0;
}