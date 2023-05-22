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


void remaining_time(ListHead* l){
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

//Implement one step of simulation
void FakeOS_simStep(FakeOS* os){
    
    printf("************** TIME: %08d **************\n", os->timer);
    printf("number of active CPUs %d/%d\n", os->cpu.size,os->tot_num_cpu);

    //TODO riscrivere tutto
    return;
}