#include <stdio.h>
#include "bool.h"
#include "tools.h"
#include "status.h"
#include "instructions.h"
#include "bubbling.h"
#include "forwarding.h"
#include "memoryStage.h"
#include "writebackStage.h"
#include "registers.h"
#include "memory.h"

/* prototypes of Functions only called within this file */
static unsigned int mem_control(bool *memerror);
static unsigned int mem_data(unsigned int icode);
static int mem_addr(unsigned int icode);
static bool mem_read(unsigned int icode);
static bool mem_write(unsigned int icode);
static void printMregister();

//M register holds the input for the fetch stage.
//It is only accessible from this file. (static)
static mregister M;

// Function: getMregister
// Description: Returns a copy of the M register
// Params: none
// Returns: mregister
// Modifies: none
mregister getMregister()
{
    return M;
}

void memoryStage(forwardType *forward, statusType *status, bubbleType *bubble)
{
    bool memerror = FALSE;
    forward->M_valE = M.valE;
    forward->M_dstM = M.dstM;
    forward->M_dstE = M.dstE;
    forward->M_Cnd = M.Cnd;
    forward->M_icode = M.icode;
    forward->M_valA = M.valA;
    bubble->M_icode = M.icode;
    unsigned int valM = mem_control(&memerror);
    unsigned int stat = M.stat;
     
    if(memerror)
        stat = ADR;
    
    forward->m_valM = valM;
    status->m_stat = stat;
    
    if(!W_stall(*status))
    {
        updateWregister(stat, M.icode, M.valE, valM, M.dstE, M.dstM);
    }
}

// Function: W_stall
// Description: determines whether W Register needs stalled
// Params: status- Status struct
// Returns: bool- TRUE if W Register needs stalled
// Modifies: none
bool W_stall(statusType status)
{
    switch (status.W_stat)
    {
        case ADR:
        case INS:
        case HLT:
        return TRUE;
    }
    return FALSE;
}
// Function: updateMregister
// Description: Sets specified values of M register
// Params: stat -current pipeline status    icode -current instr
//         Cnd  -current condition          valE  -valE of current instr
//         valA -valA of current instr      dstE  -dstE of current instr
//         dstM -dstM of current instr
// Returns: none
// Modifies: M Register
void updateMregister(unsigned int stat, unsigned int icode, unsigned int Cnd, 
                     unsigned int valE, unsigned int valA, unsigned int dstE, 
                     unsigned int dstM)
{
    M.stat = stat;
    M.icode = icode;
    M.Cnd = Cnd;
    M.valE = valE;
    M.valA = valA;
    M.dstE = dstE;
    M.dstM = dstM;
}

// Function: mem_control
// Description: performs memory read/write depending
//              on the function called
// Params: bool *memerror
// Returns: values from memory (if read)
//          M.valA (if write)
// Modifies: none
unsigned int mem_control(bool *memerror)
{
    if(mem_read(M.icode))
    {
        int addr = mem_addr(M.icode);
        return getWord(addr, memerror);
    }
    if(mem_write(M.icode))
    {
        int data = mem_data(M.icode);
        int addr = mem_addr(M.icode);
        
        unsigned char byte0 = getByteNumber(0, data);
        unsigned char byte1 = getByteNumber(1, data);
        unsigned char byte2 = getByteNumber(2, data);
        unsigned char byte3 = getByteNumber(3, data);
        putByte(addr, byte0, memerror);
        putByte(addr+1, byte1, memerror);
        putByte(addr+2, byte2, memerror);
        putByte(addr+3, byte3, memerror);
        
    }
    return M.valA;
}

// Function: mem_addr
// Description: A select line that determines what the address
//              of the memory read/write is
// Params: icode
// Returns: address of memory read/write
// Modifies: none
int mem_addr(unsigned int icode)
{
    if(icode == RMMOVL || icode == PUSHL || icode == CALL ||
        icode == MRMOVL)
        return M.valE;
    if(icode == POPL || icode == RET)
        return M.valA;
}

// Function: mem_data
// Description: determines what data will be written to memory
// Params: icode
// Returns: int -data to be written to memory
// Modifies: none
unsigned int mem_data(unsigned int icode)
{
    if(icode == RMMOVL || icode == PUSHL || M.icode == CALL)
        return M.valA;
}

// Function: mem_read
// Description: Determines whether memory needs read
// Params: none
// Returns: TRUE or FALSE depending on memory needing read
// Modifies: none
bool mem_read(unsigned int icode)
{
    return (icode == MRMOVL || icode == POPL || icode == RET);
}

// Function: mem_write
// Description: Determines whether memory needs written
// Params: none
// Returns: TRUE or FALSE depending on memory needing written
// Modifies: none
bool mem_write(unsigned int icode)
{
    return (icode == RMMOVL || icode == PUSHL || icode == CALL);
}

// Function: clearFregister
// Description: Clears the M register
// Params: none
// Returns: void
// Modifies: M
void clearMregister()
{
    M.stat = AOK;
    M.icode = NOP;
    clearBuffer((char *) &M, sizeof(M));
}

