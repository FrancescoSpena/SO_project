#include "linked_list.h"
#pragma once

typedef enum{RUNNING=2, STOP=3} StatusCpu;

typedef struct{
    ListItem* list;
    StatusCpu status;
}FakeCPU;