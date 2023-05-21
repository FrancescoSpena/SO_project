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
    os->num_cpu=num_cpu;

    for(int i=0; i < os->num_cpu; i++){
        FakeCPU* cpu = (FakeCPU*)malloc(sizeof(FakeCPU));
        cpu->list->next = cpu->list->prev = 0;
        cpu->status=STOP;
        List_pushBack(&os->cpu,(ListItem*)cpu);
    }
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

//Implement one step of simulation
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


    // if one process in running exist 
    // decrement the duration of running
    // if event over, destroy event
    // and reschedule process
    // if last event, destroy running
    FakePCB* running = (FakePCB*)os->running.first;
    printf("\trunning pid: %d\n", running ? running->pid:-1);
    if(running){
        ProcessEvent* e = (ProcessEvent*)running->events.first;
        //destroy all if not type CPU
        assert(e->type == CPU);
        e->duration--;
        printf("\t\tremaining time:%d\n",e->duration);
        if(e->duration == 0){
            printf("\t\tend burst\n");
            List_popFront(&running->events);
            free(e);
            if(!running->events.first){
                printf("\t\tend process\n");
                //kill process
                free(running);
            }else{
                //consume an event at each step
                e = (ProcessEvent*)running->events.first;
                switch(e->type){
                    case CPU:
                        printf("\t\tmove to ready\n");
                        List_pushBack(&os->ready, (ListItem*) running);
                        break;
                    case IO:  
                        printf("\t\tmove to waiting\n");
                        List_pushBack(&os->waiting, (ListItem*) running);
                        break;
                }
            }
            //Delete the first to the running list
            List_popFront(&os->running);
        }
    }

    //if not running process call the scheduler
    if(os->schedule_fn && !os->running.first){
        (*os->schedule_fn)(os,os->schedule_args);
    }

    // if running not defined and ready queue not empty
    // put the first in ready to run
    if(!os->running.first && os->ready.first){
        ListItem* first_ready = List_popFront(&os->ready);
        List_pushBack(&os->running,first_ready);
    }

    ++os->timer;

}