#include "../include/scheduler.h"

/*
##########internal function###############
*/


//Return a process with min prediction burst 
FakePCB* minBurstPrediction(ListHead* ready){
    ListItem* aux = ready->first;
    FakePCB* min = 0;
    while(aux){
        FakePCB* pcb = (FakePCB*)aux;
        if(!min || pcb->prediction_time < min->prediction_time){
            min = pcb;
        }
        aux=aux->next;
    }
    return (FakePCB*)List_detach(ready,(ListItem*)min);
}

void updatePrediction(ListHead* ready, SchedSJFArgs* args){
    ListItem* aux = ready->first;
    while(aux){
        FakePCB* pcb = (FakePCB*)aux;
        #ifdef SCHEDULER
        printf("pid: %d, duration time: %d\n", pcb->pid,pcb->duration_time);
        #endif
        if(pcb->prediction_time == 0 || pcb->update==0){
            pcb->prediction_time = args->alpha * pcb->duration_time + (1 - args->alpha) * pcb->prediction_time;
            pcb->update=1;
        }
        #ifdef DEBUG
        printf("pid: %d, new prediction: %f\n", pcb->pid,pcb->prediction_time);
        #endif
        aux=aux->next;
    }
    return;
}


//routine to free cpu
void choiceProcessFreeCPU(FakeOS *os, SchedSJFArgs* args){
    updatePrediction(&os->ready,args);
    FakePCB* min = minBurstPrediction(&os->ready);
    #ifdef SCHEDULER
    printf("min pid: %d, pred: %f\n", min->pid,min->prediction_time);
    #endif
    if(min->events.first){
        ProcessEvent* e = (ProcessEvent*)min->events.first;
        if(e->duration > args->curr_quantum && e->type == CPU){
            ProcessEvent* qe = (ProcessEvent*)malloc(sizeof(ProcessEvent));
            qe->list.prev=qe->list.next=0;
            qe->type=CPU;
            qe->duration=args->curr_quantum;
            e->duration-=args->curr_quantum;
            List_pushFront(&min->events,(ListItem*)qe);
        }
    }
    min->start_time=os->timer;
    List_pushBack(&os->running,(ListItem*)min);
    return;
}

/*
##########external function###############
*/

//scheduler
void schedSJF(FakeOS *os, void *args_){
    if(!os->ready.first){
        #ifdef SCHEDULER
        printf("[Scheduler] no ready processes\n");
        #endif
        return;
    }
    SchedSJFArgs* args = (SchedSJFArgs*)args_;

    if(os->running.size < os->tot_num_cpu){
        choiceProcessFreeCPU(os,args);
        return;
    }else return;
}