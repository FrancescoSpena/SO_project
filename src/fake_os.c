#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "fake_os.h"

void FakeOS_init(FakeOS *os){
    List_init(&os->running);
    List_init(&os->ready);
    List_init(&os->waiting);
    os->timer=0;
    os->schedule_fn=0;
    os->schedule_args=0;
    os->num_cpu=0;
    List_init(&os->processes);
}

//Return a pid of indexed process in running
int FakeOS_pidToList(ListHead l, int index){
    ListItem* aux = l.first;
    int i = 0;
    while(aux){
        FakePCB* pcb = (FakePCB*)aux;
        if(i == index) return pcb->pid;
        ++i;
        aux=aux->next;
    }
}

void FakeOS_createProcess(FakeOS* os, FakeProcess* p){
    // sanity check
    assert(p->arrival_time==os->timer && "time mismatch in creation");
    //check if the running, ready or waiting list have the same pid
    for(int i=0; i < os->num_cpu; i++){
        assert(FakeOS_pidToList(os->running,i) != p->pid);
        assert(FakeOS_pidToList(os->ready,i) != p->pid);
        assert(FakeOS_pidToList(os->waiting,i) != p->pid);
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

void FakeOS_simStep(FakeOS* os){
    printf("************** TIME: %08d **************\n", os->timer);

    //scan process waiting to be started
    //and create all processes starting now
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

    // scan waiting list, and put in ready all items whose event terminates
    aux=os->waiting.first;
    while(aux) {
        FakePCB* pcb=(FakePCB*)aux;
        aux=aux->next;
        ProcessEvent* e=(ProcessEvent*) pcb->events.first;
        printf("\twaiting pid: %d\n", pcb->pid);
        assert(e->type==IO);
        e->duration--;
        printf("\t\tremaining time:%d\n",e->duration);
        if (e->duration==0){
            printf("\t\tend burst\n");
            List_popFront(&pcb->events);
            free(e);
            List_detach(&os->waiting, (ListItem*)pcb);
            if (! pcb->events.first) {
                // kill process
                printf("\t\tend process\n");
                free(pcb);
            } else {
                //handle next event
                e=(ProcessEvent*) pcb->events.first;
                switch (e->type){
                case CPU:
                    printf("\t\tmove to ready\n");
                    List_pushBack(&os->ready, (ListItem*) pcb);
                    break;
                case IO:
                    printf("\t\tmove to waiting\n");
                    List_pushBack(&os->waiting, (ListItem*) pcb);
                    break;
                }
            }
        }
    }

    //TODO insert part to running

}