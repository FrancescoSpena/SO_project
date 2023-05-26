#include "../include/scheduler.h"

//Return the process with min burst, remove the min burst process to the list
ListItem *minBurst(ListHead *ready){
    if(ready->first == 0) return 0;

    ListItem* aux = (ListItem*)ready->first;
    int min = 1000;
    FakePCB* min_proc = (FakePCB*)malloc(sizeof(FakePCB));
    assert(min_proc);
    while(aux){
        FakePCB* r = (FakePCB*)aux;
        if(r->events.first != 0){
            ProcessEvent* e = (ProcessEvent*)r->events.first;
            if(e->type == CPU && e->duration < min){
                min = e->duration;
                min_proc = r;
            }
        }
        aux=aux->next;
    }
    return List_detach(ready,(ListItem*)min_proc);
}

//Update value of quantum
void updateQuantum(SchedSJFArgs *a){
    a->next_quantum = a->alpha * a->curr_quantum + (1 - a->alpha) * a->pred_quantum;
}

//routine for cpu busy
void cpuBusy(FakeOS* os){
    if(os == 0) return;
    ListItem* aux = os->running.first;
    int max = 0;
    FakePCB* change = (FakePCB*)malloc(sizeof(FakePCB));
    assert(change);
    while(aux){
        FakePCB* run = (FakePCB*)aux;
        if(run->events.first){
            ProcessEvent* e_run = (ProcessEvent*)run->events.first;
            if(e_run->type == CPU && e_run->duration > max){
                max = e_run->duration;
                change = run;
            }
        }
        aux=aux->next;
    }

    if(max > 0){
        FakePCB* min = (FakePCB*)minBurst(&os->ready);
        printf("pid max: %d, pid min: %d\n", change->pid,min->pid);
        ProcessEvent* e_change = (ProcessEvent*)change->events.first;
        if(!min){
            printf("Process NULL\n");
            return;
        }
        if(!min->events.first){
            printf("Process not event\n");
            return;
        }
        ProcessEvent* e_min = (ProcessEvent*)min->events.first;
        if(e_min->type != CPU){
            printf("No CPU\n");
            return;
        }
        if(e_change->duration > e_min->duration){
            //if change detach "change" to run and add to ready 
            //Add min process to running
            ListItem* ret = List_detach(&os->running,(ListItem*)change);
            List_pushBack(&os->running,(ListItem*)min);
            List_pushBack(&os->ready,(ListItem*)ret);
            printf("Process change\n");
            return;
        }
        //if not change the min process re-add to ready 
        List_pushBack(&os->ready,(ListItem*)min);
        printf("Process not change\n");
    }
    return;
}

//routine for timeout 
int timeoutQuantum(FakeOS* os, FakePCB* run, int curr_quantum){
    if(os == 0 || curr_quantum < 0 || run == 0) return -1;
    if(run->events.first){
        ProcessEvent* e = (ProcessEvent*)run->events.first;
        if(e->type == CPU){
            if(e->duration > curr_quantum){
                ProcessEvent* qe = (ProcessEvent*)malloc(sizeof(ProcessEvent));
                qe->list.prev=qe->list.next=0;
                qe->type=CPU;
                /*
                the event remains in the cpu for a duration 
                equal to the quantum, it is then updated 
                with what remains
                */
                e->duration-=curr_quantum;
                qe->duration=e->duration;
                List_pushFront(&run->events,(ListItem*)qe);

                ListItem* ret = List_detach(&os->running,(ListItem*)run);
                FakePCB* min = (FakePCB*)minBurst(&os->ready);
                if(!min){
                    printf("Process NULL\n");
                    return -1;
                }
                if(!min->events.first){
                    printf("Process not event\n");
                    return -1;
                }
                ProcessEvent* e_min = (ProcessEvent*)min->events.first;
                if(e_min->type != CPU){
                    printf("No CPU\n");
                    return -1;
                }
                List_pushBack(&os->running,(ListItem*)min);
                List_pushBack(&os->ready,(ListItem*)ret);
                return 1;
            }else return 0;
        }else printf("pid: %d, no event cpu\n", run->pid);
    }else printf("pid: %d, no event found\n", run->pid);
    return -1;
}

void choiceProcessFreeCPU(FakeOS *os){
    if(os == 0) return;
    printf("Free CPU\n");
    FakePCB* p = (FakePCB*)List_popFront(&os->ready);
    List_pushBack(&os->running,(ListItem*)p);
    return;
}

void choiceProcessBusyCPU(FakeOS *os, int curr_quantum){
    if(os == 0 || curr_quantum < 0) return;
    printf("busy CPU\n");
    printf("curr quantum = %d\n", curr_quantum);
    int flag = 0;
    //Preemptive
    ListItem* aux = os->running.first;
    while(aux){
        FakePCB* run = (FakePCB*)aux;
        if(timeoutQuantum(os,run,curr_quantum)){
            printf("pid: %d, timeout\n",run->pid);
            flag = 1;
        }
        aux=aux->next;
    }

    if(flag == 1) return;

    //CPU busy
    cpuBusy(os);
    return;
    
}

void schedSJF(FakeOS *os, void *args_){
    if(os == 0) return;
    SchedSJFArgs* args = (SchedSJFArgs*)args_;

    if(os->ready.first == 0){
        printf("No ready processes\n");
        return;
    }

    updateQuantum(args);

    if(os->running.size < os->tot_num_cpu){
        choiceProcessFreeCPU(os);
    }else{
        choiceProcessBusyCPU(os,args->curr_quantum);
    }
}