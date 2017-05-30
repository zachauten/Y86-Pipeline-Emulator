#include "bool.h"
#include "tools.h"
#include <stdio.h>
#include "instructions.h"
#include "forwarding.h"
#include "status.h"
#include "writebackStage.h"

/* prototypes of Functions only called within this file */
static void printWregister();

// W register hold the input for the writeback stage
// It is only accessible from this file. (static)
static wregister W;

// Function: getWregister
// Description: Returns a copy of the W register
// Params: none
// Returns: wregister
// Modifies: none
wregister getWregister()
{
    return W;
}

// Function: writebackStage
// Description: Handles the logic for the writeback stage 
// Params: forwardType struct   statusType struct
// Returns: bool - stat
// Modifies: 
bool writebackStage(forwardType *forward, statusType *status)
{
    switch(W.stat)
    {
        case INS:
            printf("Invalid instruction\n");
            dumpProgramRegisters();
            dumpProcessorRegisters();
            dumpMemory();
            return TRUE;
        case ADR:
            printf("Invalid memory address\n");
            dumpProgramRegisters();
            dumpProcessorRegisters();
            dumpMemory();
            return TRUE;
        case HLT:
            return TRUE;
    }
    
    forward->W_dstE = W.dstE;
    forward->W_dstM = W.dstM;
    forward->W_valE = W.valE;
    forward->W_valM = W.valM;
    forward->W_icode = W.icode;

    if(W.icode == DUMP)
    {
        unsigned char temp = W.valE;
        if((temp & 0x1) == 0x1)
            dumpProgramRegisters();
        if((temp & 0x2) == 0x2)
            dumpProcessorRegisters();
        if((temp & 0x4) == 0x4)
            dumpMemory();
    }
    status->W_stat = W.stat;
    setRegister(W.dstE, W.valE);
    setRegister(W.dstM, W.valM);
    return FALSE;
}

// Function: updateWregister
// Description: updates current values of the w register
// Params: stat -current pipeline status   icode -current instr
//         valE -valE of current instr     valM  -valM of current instr
//         dstE -dstE of current instr     dstM  -dstM of current instr
// Returns: void
// Modifies: W Register  
void updateWregister(unsigned int stat, unsigned int icode, unsigned int valE, 
                     unsigned int valM, unsigned int dstE, unsigned int dstM)
{
    W.stat = stat;
    W.icode = icode;
    W.valE = valE;
    W.valM = valM;
    W.dstE = dstE;
    W.dstM = dstM;
}
// Function: clearWregister
// Description: Clears contents of the W register
// Params: none
// Returns: void
// Modifies: W
void clearWregister()
{
    W.stat = AOK;
    W.icode = NOP;
    clearBuffer((char *) &W, sizeof(W));
}
