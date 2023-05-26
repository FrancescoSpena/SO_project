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
    int i = 0;
    while(aux){
        FakePCB* run = (FakePCB*)aux;
        if(run->events.first != 0){
            ProcessEvent* e = (ProcessEvent*)run->events.first;
            if(e->type == CPU && e->duration > curr_quantum){
                printf("pid: %d, timeout quantum of %d\n", run->pid,curr_quantum);
                ProcessEvent* qe = (ProcessEvent*)malloc(sizeof(ProcessEvent));
                qe->list.prev=qe->list.next=0; 
                qe->type=CPU;
                qe->duration=curr_quantum;
                e->duration-=curr_quantum;
                List_pushFront(&run->events, (ListItem*)qe);
                //Remove to running
                ListItem* ret = List_detach(&os->running,(ListItem*)run);
                printf("Remove process to running\n");

                //Restore aux
                //Fare funzioni separate e fare in modo che
                //quando si ha un timeout venga notificato e si riparte 
                //da capo per il controllo (sperando di non avere problemi)
                //con gli ultimi aggiunti
                aux=os->running.first;

                //Take process min burst
                if(!os->ready.first){
                    return;
                }
                FakePCB* min = (FakePCB*)minBurst(&os->ready);
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
                //Add to running
                printf("Add process pid min: %d to running\n", min->pid);
                List_pushBack(&os->running,(ListItem*)min);
                List_pushBack(&os->ready,(ListItem*)ret);
                
                //Active flag
                flag = 1;
            }else printf("Process in run event IO\n");
        }else printf("Process in run not event\n");
        aux=aux->next;
        i++;
    }
    printf("aux 0\n");

    if(flag == 1) return;

    //CPU busy
    aux = os->running.first;
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
        //if not change the min process readd to ready because do a pop
        //of list
        List_pushBack(&os->ready,(ListItem*)min);
        printf("Process not change\n");
    }
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