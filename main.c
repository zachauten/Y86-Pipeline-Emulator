#include <stdio.h>
#include "bool.h"
#include "tools.h"
#include "memory.h"
#include "dump.h"
#include "loader.h"
#include "status.h"
#include "instructions.h"
#include "forwarding.h"
#include "bubbling.h"
#include "fetchStage.h"
#include "decodeStage.h"
#include "executeStage.h"
#include "memoryStage.h"
#include "writebackStage.h"


void initialize();

int main(int argc, char * args[])
{
    int clockCount = 0;
    bool stop = FALSE;
    forwardType forward;
    bubbleType bubble;
    statusType status;

    initialize();
    
    if(!load(argc, args))
    {
        dumpMemory();
        return 0;
    }

    while(!stop)
    {
        stop = writebackStage(&forward, &status);
        memoryStage(&forward, &status, &bubble);
        executeStage(&forward, &status, &bubble);
        decodeStage(forward, &bubble);
        fetchStage(&forward, bubble);
        clockCount++;
    }
    printf("\nTotal clock cycles = %d\n", clockCount);

    return 0;
}

void initialize()
{
    initializeFuncPtrArray();

    clearMemory();
    clearRegisters();
    
    clearFregister();
    clearDregister();
    clearEregister();
    clearMregister();
    clearWregister();
}


