#include "../include/scheduler.h"


void choiceProcessFreeCPU(FakeOS *os){
    if(os == NULL) return;
    printf("CPU busy %d/%d\n", os->cpu.size,os->tot_num_cpu);
    printf("Prendo un nuovo processo ready\n");
    FakePCB* new_run = (FakePCB*)List_popFront(&os->ready);
    printf("Controllo se è CPU\n");
    ProcessEvent* e = (ProcessEvent*)new_run->events.first;
    assert(e->type == CPU);
    printf("Aggiungo alla lista dei running, adesso size = %d\n", os->running.size);
    List_pushBack(&os->running,(ListItem*)new_run);
    printf("Size lista running = %d\n", os->running.size);
    printf("Creo una nuova CPU\n");
    FakeCPU* new_cpu = (FakeCPU*)malloc(sizeof(FakeCPU));
    new_cpu->execution = new_run;
    printf("Aggiungo alla lista delle cpu, adesso size = %d\n", os->cpu.size);
    List_pushBack(&os->cpu,(ListItem*)new_cpu);
    printf("Size lista cpu = %d\n", os->cpu.size);
    return;
}

void schedSJF(FakeOS *os, void *args_){
    if(os == 0) return;
    SchedSJFArgs* args = (SchedSJFArgs*)args_;

    if(os->ready.first == 0) return;

    if(os->cpu.size < os->tot_num_cpu) choiceProcessFreeCPU(os);
    else printf("not implemented\n");
    return;
}