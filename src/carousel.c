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

FakePCB* minBurst(ListHead* ready){
    ListItem* aux = ready->first;
    FakePCB* ret = (FakePCB*)malloc(sizeof(FakePCB));
    int min = 1000;
    while(aux){
        FakePCB* pcb = (FakePCB*)aux;
        if(pcb->events.first != NULL){
            ProcessEvent* e = (ProcessEvent*)pcb->events.first;
            if(e->type == CPU){
                if(e->duration < min){
                    min = e->duration;
                    ret->events = pcb->events;
                    ret->list = pcb->list;
                    ret->pid = pcb->pid;
                }
            }
        }
        aux=aux->next;
    }
    List_detach(ready,(ListItem*)ret);
    return ret;
}

void schedSJF(FakeOS *os, void* args_){
    SchedSJFArgs *args = (SchedSJFArgs*)args_;

    // look for the first process in ready
    // if none, return
    if(!os->ready.first){
        return;
    }

    //Add quantum
    /*
    La cosa che rimane da fare è quella di gestire il caso in cui 
    i processi che sono in esecuzione hanno terminato il proprio
    quantum (che ad ogni giro di giostra viene aggioranto in 
    termini di durata).
    Per fare ciò devo controllare per ogni cpu se il processo 
    attualmente in esecuzione ha passato un tempo di esecuzione = 
    quantum impostato. Nel caso questo sia vero devo scegliere
    un nuovo processo dalla ready queue che abbia minimo burst e 
    sostituirlo con quello che ha superato il suo quantum.
    */

    //If same cpu in free add a process
    printf("CPU occupate = %d\n", os->busy_cpu);
    if(os->busy_cpu < os->tot_num_cpu){
        printf("Qualche CPU libera\n");
        FakePCB* run = (FakePCB*)List_popFront(&os->ready);
        printf("Aggiungo il processo alla running queue\n");
        List_pushBack(&os->running,(ListItem*)run);
        printf("Scheduler, size running list: %d\n", os->running.size);
        FakeCPU* cpu = (FakeCPU*)malloc(sizeof(FakeCPU));
        assert(cpu);
        cpu->execution=run;
        os->busy_cpu++;
        printf("Aggiungo una nuova cpu\n");
        List_pushBack(&os->cpu,(ListItem*)cpu);
        printf("Scheduler, size cpu list: %d\n", os->cpu.size);
        printf("processo scelto\n");
    }
    //If all cpu is busy, find to ready queue the process with
    //min burst and change with the process'burst to high.
    else{
        printf("Sono tutte occupate\n");
        //Take a process with min burst
        FakePCB* min = minBurst(&os->ready);
        assert(min->events.first);
        ProcessEvent* e = (ProcessEvent*)min->events.first;
        assert(e->type == CPU);
        
        //Check if the burst of process in execution > min burst 
        //Save process with max burst
        int flag = 0;
        int max = 0;
        FakePCB* ret = (FakePCB*)malloc(sizeof(FakePCB));
        assert(ret);
        ListItem* aux = os->running.first;
    
        while(aux){
            FakePCB* pcb = (FakePCB*)aux;
            if(pcb->events.first){
                ProcessEvent* pe = (ProcessEvent*)pcb->events.first;
                if(pe->type == CPU){
                    if(pe->duration > e->duration){
                        flag = 1;
                        if(pe->duration > max){
                            max = pe->duration;
                            ret->events = pcb->events;
                            ret->list = pcb->list;
                            ret->pid = pcb->pid;
                        }
                    }
                }
            }
            aux=aux->next;
        }

        //If find process with burst > min burst, remove to
        //running queue and add the new min process
        if(flag == 1){
            printf("Effettuo una sostituzione\n");
            List_detach(&os->running,(ListItem*)ret);
            List_detach(&os->cpu,(ListItem*)ret);
            List_pushBack(&os->running,(ListItem*)min);
            FakeCPU* cpu = (FakeCPU*)malloc(sizeof(FakeCPU));
            cpu->execution = min;
            List_pushBack(&os->cpu,(ListItem*)min);
            return;
        } 
        printf("Processo scelto (caso non libere) ma No sostituito\n");
    }
    return;
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
    srr_args.curr_quantum = srr_args.pred_quantum = 2;
    srr_args.alpha = 0.3;
    srr_args.next_quantum = 0;
    os.schedule_fn=schedSJF;
    os.schedule_args=&srr_args;

    //argv[1] = num_core
    for(int i=2; i < argc; i++){
        FakeProcess new_process;
        int num_events = FakeProcess_load(&new_process,argv[i]);
        printf("loading [%s], pid: %d, events:%d\n",
           argv[i], new_process.pid, num_events);
        
        if(num_events){
            FakeProcess* new_process_ptr=(FakeProcess*)malloc(sizeof(FakeProcess));
            *new_process_ptr = new_process;
            List_pushBack(&os.processes,(ListItem*)new_process_ptr);
        }
    }
    printf("\nnum core selected %d\n", os.tot_num_cpu);
    printf("num processes in queue %d\n", os.processes.size);

    while(os.running.first
        || os.ready.first
        || os.waiting.first
        || os.processes.first){
        FakeOS_simStep(&os);
    }

    return 0;
}