#include "../include/scheduler.h"

//Update value of quantum
void updateQuantum(SchedSJFArgs *args){
    args->next_quantum = args->alpha * args->curr_quantum + (1 - args->alpha) * args->pred_quantum;
}

FakePCB *minBurst(ListHead *ready){
    ListItem *aux = ready->first;
    FakePCB *ret = (FakePCB *)malloc(sizeof(FakePCB));
    int min = 1000;
    while (aux){
        FakePCB *pcb = (FakePCB *)aux;
        if (pcb->events.first != NULL){
            ProcessEvent *e = (ProcessEvent *)pcb->events.first;
            if (e->type == CPU){
                if (e->duration < min){
                    min = e->duration;
                    ret->events = pcb->events;
                    ret->list = pcb->list;
                    ret->pid = pcb->pid;
                }
            }
        }
        aux = aux->next;
    }
    List_detach(ready, (ListItem *)ret);
    return ret;
}

//Update all event of running queue
void updateEvent(ListHead *cpu){
    if(!cpu || cpu->size <= 0) return;
    ListItem* aux = cpu->first;
    while(aux){
        FakeCPU* cpu = (FakeCPU*)aux;
        cpu->time_exection++;
        aux=aux->next;
    }
    return;
}

//Change a process max burst with min
void changeProcess(FakeOS* os, FakePCB* min){
    assert(min->events.first);
    ProcessEvent *e = (ProcessEvent *)min->events.first;
    assert(e->type == CPU);

    // Check if the burst of process in execution > min burst
    // Save process with max burst
    int flag = 0;
    int max = 0;
    FakePCB *ret = (FakePCB *)malloc(sizeof(FakePCB));
    assert(ret);
    ListItem *aux = os->running.first;

    while (aux){
        FakePCB *pcb = (FakePCB *)aux;
        if (pcb->events.first){
            ProcessEvent *pe = (ProcessEvent *)pcb->events.first;
            if (pe->type == CPU){
                if (pe->duration > e->duration){
                    flag = 1;
                    if (pe->duration > max){
                        max = pe->duration;
                        ret->events = pcb->events;
                        ret->list = pcb->list;
                        ret->pid = pcb->pid;
                    }
                }
            }
        }
        aux = aux->next;
    }

    // If find process with burst > min burst, remove to
    // running queue and add the new min process
    if (flag == 1){
        printf("Effettuo una sostituzione\n");
        List_detach(&os->running, (ListItem *)ret);
        List_detach(&os->cpu, (ListItem *)ret);
        List_pushBack(&os->running, (ListItem *)min);
        FakeCPU *cpu = (FakeCPU *)malloc(sizeof(FakeCPU));
        cpu->execution = min;
        List_pushBack(&os->cpu, (ListItem *)min);
        return;
    }
    printf("Processo scelto (caso non libere) ma No sostituito\n");
}

// Routine free cpu
void choiceProcessFreeCPU(FakeOS *os){
    printf("Qualche CPU libera\n");
    FakePCB *run = (FakePCB *)List_popFront(&os->ready);
    assert(run);
    printf("Aggiungo il processo alla running queue\n");
    List_pushBack(&os->running, (ListItem *)run);
    printf("Scheduler, size running list: %d\n", os->running.size);
    FakeCPU *cpu = (FakeCPU *)malloc(sizeof(FakeCPU));
    assert(cpu);
    cpu->execution = run;
    os->busy_cpu++;
    printf("Aggiungo una nuova cpu\n");
    List_pushBack(&os->cpu, (ListItem *)cpu);
    printf("Scheduler, size cpu list: %d\n", os->cpu.size);
    printf("Processo scelto\n");
}

// Routine busy cpu
void choiceProcessBusyCPU(FakeOS *os, int curr_quantum){
    if(!os) return;
    printf("Sono tutte occupate\n");

    FakePCB *min = minBurst(&os->ready);

    //routine quantum
    int flag = 0;
    ListItem* aux = os->cpu.first;
    while(aux){
        FakeCPU* cpu = (FakeCPU*)aux;
        if(cpu->time_exection == curr_quantum){
            printf("Processo che ha finito il suo burst\n");
            List_detach(&os->running,(ListItem*)cpu->execution);
            cpu->execution = min;
            List_pushBack(&os->running,(ListItem*)min);
            flag = 1;
            printf("Processo sostituito con il minimo\n");
        }
        aux=aux->next;
    }

    if(flag == 1) return;
    changeProcess(os,min);
    return;
}

//Scheduler SJF preemptive
void schedSJF(FakeOS *os, void *args_){
    SchedSJFArgs *args = (SchedSJFArgs *)args_;

    printf("CPU occupate = %d\n", os->busy_cpu);
    
    if (!os->ready.first){
        return;
    }

    updateEvent(&os->running);
    updateQuantum(args);

    if (os->busy_cpu < os->tot_num_cpu){
        choiceProcessFreeCPU(os);
    }else{
        choiceProcessBusyCPU(os,args->curr_quantum);
    }
    return;
}