#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../include/fake_os.h"

#define NUM_CPU 2

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

    //check if all cpu are busy
    //if the number of busy < total num of cpu
    //_______________
        //Add process to ready list to running 
        //in cpu, set the status = running and 
        //add to running list
    //_______________
        /*
        For each cpu check the status and if the 
        cpu burst of process in running is > 
        new select process cpu burst change else 
        do nothing.
        */
    //_______________

}

int main(int argc, char** argv){
    printf("Go the carousel\n");
    FakeOS_init(&os, NUM_CPU);

    //TODO scheduler args


    return 0;
}