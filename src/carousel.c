#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../include/fake_os.h"
#include "../include/scheduler.h"

FakeOS os;

int main(int argc, char **argv){

    if (argc <= 1){
        printf("usage: %s <num_core> <processes>", argv[0]);
        return 0;
    }

    printf("Go the carousel\n");
    int num_cpu = atoi(argv[1]);
    FakeOS_init(&os, num_cpu);

    // scheduler args
    SchedSJFArgs srr_args;
    srr_args.curr_quantum = srr_args.pred_quantum = 2;
    srr_args.alpha = 0.3;
    srr_args.next_quantum = 0;
    os.schedule_fn = schedSJF;
    os.schedule_args = &srr_args;

    // argv[1] = num_core
    for (int i = 2; i < argc; i++){
        FakeProcess new_process;
        int num_events = FakeProcess_load(&new_process, argv[i]);
        printf("loading [%s], pid: %d, events:%d\n",
               argv[i], new_process.pid, num_events);

        if (num_events){
            FakeProcess *new_process_ptr = (FakeProcess *)malloc(sizeof(FakeProcess));
            *new_process_ptr = new_process;
            List_pushBack(&os.processes, (ListItem *)new_process_ptr);
        }
    }
    printf("\nnum core selected %d\n", os.tot_num_cpu);
    printf("num processes in queue %d\n", os.processes.size);

    while (os.running.first || 
           os.ready.first || 
           os.waiting.first || 
           os.processes.first){
        FakeOS_simStep(&os);
    }
    printf("END\n");
    return 0;
}