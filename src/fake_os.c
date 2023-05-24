#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../include/fake_os.h"


//Inizialized a FakeOS
void FakeOS_init(FakeOS *os, int num_cpu){
    List_init(&os->cpu);
    List_init(&os->running);
    List_init(&os->ready);
    List_init(&os->waiting);
    List_init(&os->processes);
    
    os->timer=0;
    os->schedule_fn=0;
    os->schedule_args=0;
    os->tot_num_cpu=num_cpu;
    os->busy_cpu=0;
}


//Create a process (check if the process already exists)
void FakeOS_createProcess(FakeOS* os, FakeProcess* p){
    // sanity check
    assert(p->arrival_time==os->timer && "time mismatch in creation");
    //check if the running, ready or waiting list have the same pid
    
    ListItem* aux=os->running.first;
    while(aux){
        FakePCB* pcb=(FakePCB*)aux;
        assert(pcb->pid!=p->pid && "pid taken");
        aux=aux->next;
    }

    aux=os->ready.first;
    while(aux){
        FakePCB* pcb=(FakePCB*)aux;
        assert(pcb->pid!=p->pid && "pid taken");
        aux=aux->next;
    }

    aux=os->waiting.first;
    while(aux){
        FakePCB* pcb=(FakePCB*)aux;
        assert(pcb->pid!=p->pid && "pid taken");
        aux=aux->next;
    }

    // all fine, no such pcb exists
    FakePCB* new_pcb=(FakePCB*) malloc(sizeof(FakePCB));
    new_pcb->list.next=new_pcb->list.prev=0;
    new_pcb->pid=p->pid;
    new_pcb->events=p->events;

    assert(new_pcb->events.first && "process without events");

    // depending on the type of the first event
    // we put the process either in ready or in waiting
    ProcessEvent* e=(ProcessEvent*)new_pcb->events.first;
    switch(e->type){
    case CPU:
        List_pushBack(&os->ready, (ListItem*) new_pcb);
        break;
    case IO:
        List_pushBack(&os->waiting, (ListItem*) new_pcb);
        break;
    default:
        assert(0 && "illegal resource");
        ;
    }
}

//Print remaining time of process if exist
void remainingTime(ListHead* l){
    if(l == 0) return;
    printf("\t\tsize list: %d\n", l->size);
    ListItem* aux = l->first;
    while(aux){
        FakePCB* pcb = (FakePCB*)aux;
        if(pcb->events.first != NULL){
            ProcessEvent* e = (ProcessEvent*)pcb->events.first;
            printf("\t\tpid: %d, remaining time: %d\n", pcb->pid,e->duration);
        }
        aux=aux->next;
    }
    return;
}

//Routine to organize a process
void handlerProcess(FakeOS* os){
    if(os == 0) return;
    ListItem* aux=os->processes.first;
    while (aux){
        FakeProcess* proc=(FakeProcess*)aux;
        FakeProcess* new_process=0;
        if (proc->arrival_time==os->timer){
            new_process=proc;
        }
        aux=aux->next;
        if (new_process) {
            printf("\tcreate pid:%d\n", new_process->pid);
            new_process=(FakeProcess*)List_detach(&os->processes, (ListItem*)new_process);
            FakeOS_createProcess(os, new_process);
            free(new_process);
        }
    }
    return;
}

//Print the process pid of the list
void pidProcess(ListHead* l, char* string){
    if(l == 0) return;
    ListItem* aux = l->first;
    while(aux){
        FakePCB* pcb = (FakePCB*)aux;
        printf("\t%s pid: %d\n", string, pcb->pid);
        aux=aux->next;
    }
}

//Routine for waiting process
void handlerWaitingProcess(FakeOS* os){
    if(os == 0) return;
    ListItem* aux = os->waiting.first;
    while(aux){
        FakePCB* pcb = (FakePCB*)aux;
        ProcessEvent* e = (ProcessEvent*)pcb->events.first;
        pidProcess(&os->waiting,"waiting");
        assert(e->type == IO);
        e->duration--;
        remainingTime(&os->waiting);
        if(e->duration == 0){
            printf("\t\tpid: %d, end burst\n", pcb->pid);
            List_popFront(&pcb->events);
            free(e);
            List_detach(&os->waiting, (ListItem*)pcb);
            if(!pcb->events.first){
                printf("pid: %d, end process\n", pcb->pid);
                free(pcb);
            }else{
                e = (ProcessEvent*)pcb->events.first;
                switch(e->type){
                    case CPU:
                        printf("\t\tpid: %d, event move to ready\n", pcb->pid);
                        List_pushBack(&os->ready,(ListItem*)pcb);
                        break;
                    case IO:
                        printf("\t\tpid: %d, event move to waiting\n", pcb->pid);
                        List_pushBack(&os->waiting,(ListItem*)pcb);
                        break;
                }
            }
        }
        aux=aux->next;
    }
    return;
}

//Routine for running process
void handlerRunningProcess(FakeOS* os){
    if(os == 0) return;
    //pid running process
    pidProcess(&os->running,"running");
    
    //in questo modo non sono sicuro che il processo selezionato è di tipo CPU
    /*if(!os->running.first && os->ready.first){
        List_pushBack(&os->running,(ListItem*)List_popFront(&os->ready));
        return;
    }*/

    ListItem* aux = os->running.first;
    if(aux){
        FakePCB* run = (FakePCB*)aux;
        if(run){
            ProcessEvent* e = (ProcessEvent*)run->events.first;
            assert(e->type == CPU);
            e->duration--;
            remainingTime(&os->running);
            if(e->duration == 0){
                printf("\t\tpid: %d, end burst\n", run->pid);
                List_popFront(&run->events);
                free(e);
                if(!run->events.first){
                    printf("\t\tpid: %d, end process\n", run->pid);
                    free(run);
                }else{
                    e = (ProcessEvent*)run->events.first;
                    switch(e->type){
                        case CPU:
                            printf("\t\tpid: %d, event move to ready\n", run->pid);
                            List_pushBack(&os->ready,(ListItem*)run);
                            break;
                        case IO:
                            printf("\t\tpid: %d, event move to waiting\n", run->pid);
                            List_pushBack(&os->waiting,(ListItem*)run);
                            break;
                    }
                }
                List_detach(&os->running,(ListItem*)run);
            }
        }
        aux=aux->next;
    }
}

//Implement one step of simulation
void FakeOS_simStep(FakeOS* os){
    printf("************** TIME: %08d **************\n", os->timer);
    printf("number of active CPUs %d/%d\n", os->cpu.size,os->tot_num_cpu);

    /*
    Scan process waiting to be started and create all processes
    starting now
    */

    //Routine for processes queue
    handlerProcess(os);
    printf("Complete routine 1\n");
    //Routine for waiting queue
    handlerWaitingProcess(os);
    printf("Complete routine 2\n");
    //Routine for running queue
    handlerRunningProcess(os);
    printf("Complete routine 3\n");

    printf("Call scheduler\n");
    // call schedule, if defined
    if (os->schedule_fn){
        (*os->schedule_fn)(os, os->schedule_args); 
    }

    printf("Update timer\n");
    ++os->timer;

}