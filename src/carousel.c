#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../include/fake_os.h"

FakeOS os;

typedef struct{
    int quantum;
}SchedSJFArgs;

void schedSJF(FakeOS *os, void* args_){
    SchedSJFArgs *args = (SchedSJFArgs*)args_;

    // look for the first process in ready
    // if none, return
    if(!os->ready.first){
        return;
    }

    //Calcolo la durata del prossimo quantum

    /*
    Guardo nella lista dei processi ready e scelgo quello con 
    CPU burst più piccolo. Controllo la lista dei processi running
        - Se ogni processo in running a CPU burst più piccolo di 
        quello che ho scelto non modifico niente altrimenti prendo quello
        con CPU burst più grande, lo rimuovo dalla lista di running e 
        aggiungo quello scelto
        - Aggiorno la durata del burst facendo la differenza con la durata
        del quantum calcolato
    */


}

int main(int argc, char** argv){
    printf("Go\n");
    FakeOS_init(&os);

    //TODO scheduler args


    for (int i=1; i<argc; ++i){
        FakeProcess new_process;
        int num_events=FakeProcess_load(&new_process, argv[i]);
        printf("loading [%s], pid: %d, events:%d",
            argv[i], new_process.pid, num_events);
        if (num_events) {
        FakeProcess* new_process_ptr=(FakeProcess*)malloc(sizeof(FakeProcess));
        *new_process_ptr=new_process;
        List_pushBack(&os.processes, (ListItem*)new_process_ptr);
        }
    }
    printf("num processes in queue %d\n", os.processes.size);
    while(os.running.first
            || os.ready.first
            || os.waiting.first
            || os.processes.first){
        FakeOS_simStep(&os);
    }

    return 0;
}