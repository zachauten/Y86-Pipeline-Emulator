#include "bool.h"
#include "tools.h"
#include <stdio.h>
#include "instructions.h"
#include "status.h"
#include "bubbling.h"
#include "forwarding.h"
#include "executeStage.h"
#include "memoryStage.h"
#include "registers.h"

/* prototypes of Functions only called within this file */
static int doNothing();
static int performDump();
static int performRrmovl();
static int performRmmovl();
static int performMrmovl();
static int performIrmovl();
static int performOpl();
static int performJXX();
static int performCALL();
static int performRET();
static int performPush();
static int performPop();
static void printEregister();

// E register holds the input for the execute stage.
// It is only accessible from this file. (static)
static eregister E;
int (*funcArr[16])();

// Function: getEregister
// Description: Returns a copy of the E address
// Params: none
// Returns: eregister
// Modifies: none
eregister getEregister()
{
    return E;
}

void executeStage(forwardType *forward, statusType *status, bubbleType *bubble)
{ 
    bubble->E_icode = E.icode;
    bubble->E_dstM = E.dstM;
    unsigned int valE = 0;
    unsigned int e_dstE;
    bool e_Cnd = FALSE;

    if(E.icode == RRMOVL || E.icode == JXX)
    {
        e_Cnd = calculateCnd();
    }
    if(E.stat != INS)
    {
        valE = (*funcArr[E.icode])();
    }
    if(E.icode == RRMOVL || E.icode == JXX)
    {
        e_Cnd = calculateCnd();
    }
    if((E.icode == RRMOVL) && !e_Cnd)
    {
        e_dstE = RNONE;
    }
    else
    {
        e_dstE = E.dstE;
    }

    forward->e_dstE = E.dstE;
    forward->e_valE = valE;
    bubble->e_Cnd = e_Cnd;
    if(isException(status))
    {
        setCC(OF, 0);
        setCC(ZF, 0);
        setCC(SF, 0);
        updateMregister(1, NOP, 0, 0, 0, RNONE, RNONE);
    }
    else
    {
        updateMregister(E.stat, E.icode, e_Cnd, valE, E.valA, e_dstE, E.dstM);
    }
}

// Function: isException
// Description: Determines whether memory stage should be bubbled
// Params: status -Status struct
// Returns: bool -TRUE if memory stage needs stalled
// Modifies: none
bool isException (statusType *status)
{
    switch (status->m_stat)
    {
        case ADR:
        case INS:
        case HLT:
        return TRUE;
    }
    switch (status->W_stat)
    {
        case ADR:
        case INS:
        case HLT:
        return TRUE;
    }
    return 0;
}

// Function: initializeFuncPtrArray
// Description: initializes the function ptr array
// Params: none
// Returns: void
// Modifies: funcArr[] 
void initializeFuncPtrArray()
{
    funcArr[HALT] = &doNothing;
    funcArr[NOP] = &doNothing;
    funcArr[RRMOVL] = &performRrmovl;
    funcArr[IRMOVL] = &performIrmovl;
    funcArr[RMMOVL] = &performRmmovl;
    funcArr[MRMOVL] = &performMrmovl;;
    funcArr[OPL] = &performOpl;
    funcArr[JXX] = &performJXX;
    funcArr[CALL] = &performCALL;
    funcArr[RET] = &performRET;
    funcArr[PUSHL] = &performPush;
    funcArr[POPL] = &performPop;
    funcArr[DUMP] = &performDump;
}

// Function: doNothing
// Description: does nothing in the array of function ptrs (funcArr[])
// Params: none
// Returns: 0
// Modifies: none 
int doNothing()
{
    return 0;
}

// Function: performIrmovl
// Description: Simulates IRMOVL instruction
// Params: none
// Returns: E.valC
// Modifies: none 
int performIrmovl()
{
    return E.valC;
}

// Function: calculateCnd
// Description: Determines Cnd based on condition codes
// Params: none
// Returns: bool
// Modifies: none
bool calculateCnd()
{
    unsigned int e_OF = getCC(OF);
    unsigned int e_ZF = getCC(ZF);
    unsigned int e_SF = getCC(SF);

    switch(E.ifun)
    {
        case RRFUN:
            return TRUE;
        case CMOVLE:
            return ((e_SF^e_OF)|e_ZF);
        case CMOVL:
            return (e_SF ^ e_OF);
        case CMOVE:
            return e_ZF;
        case CMOVNE:
            return !e_ZF;
        case CMOVGE:
            return (!(e_SF ^ e_OF));
        case CMOVG:
            return ((!(e_SF^ e_OF)) & (!e_ZF));
        default:
            return FALSE;
    }
}

// Function: performMrmovl
// Description: Simulates the MRMOVL instruction
// Params: none
// Returns: int- E.valB + E.valC
// Modifies: none
int performMrmovl()
{
    return E.valB + E.valC;
}

// Function performRmmovl
// Description: Simulates the RMMOVL instruction
// Params: none
// Returns int- E.valB + E.valC
// Modifies: none
int performRmmovl()
{
    return E.valB + E.valC;
}

// Function: performRrmovl
// Description: Simulates the RRMOVL instruction
// Params: none
// Returns: int- E.valA
// Modifies: none
int performRrmovl()
{
    return E.valA;
}
// Function: performOpl
// Description: Simulates OPL instruction
// Params: none
// Returns: int -result of operation
// Modifies: E.stat
int performOpl(){
    int result = 0;
    switch(E.ifun)
    {
        case ADDL:
            result = E.valB + E.valA;
            if((signed)isNegative(E.valA) == (signed)isNegative(E.valB) && ((signed)isNegative(result) != (signed)isNegative(E.valA)))
                setCC(OF, 1);
            else
                setCC(OF, 0);
            break;
        case SUBL:
            result = E.valB - E.valA;
            if(((signed)isNegative(E.valB)) != ((signed) isNegative(E.valA)) && ((signed)isNegative(E.valA)) == ((signed) isNegative(result))) 
                setCC(OF, 1);
            else
                setCC(OF, 0);
            break;
        case ANDL:
            result = E.valB & E.valA;
            setCC(OF, 0);
            break;
        case XORL:
            result = E.valB ^ E.valA;
            setCC(OF, 0);
            break;
        default:
            E.stat = INS;
            break;
    }
    setCC(ZF, result == 0);
    setCC(SF, result < 0);
    return result;
}

// Function: performJXX
// Description: Simulates the JXX instruction
// Params: none
// Returns: int- E.valC
// Modifies: none
int performJXX()
{
    return E.valC;
}

// Function performCALL
// Description: Simulates the CALL instruction
// Params: none
// Returns: int- E.valB + (-4)
// Modifies: none
int performCALL()
{
    return E.valB + (-4);
}

// Function: performRET
// Description: Simulates the RET instruction
// Params: none
// Returns: int- E.valB + 4;
// Modifies: none
int performRET()
{
    return E.valB + 4;
}

// Function: performPush
// Description: Simulates the PUSH instruction
// Params: none
// Returns: int- E.valB + (-4)
// Modifies: none
int performPush()
{
    return E.valB + (-4);
}

// Function: performPop
// Description: Simulates the POP instruction
// Params: none
// Returns: int- E.valB + 4
// Modifes: none
int performPop()
{
    return E.valB + 4;
}

// Function: performDump
// Description: simulates DUMP instruction
// Params: none
// Returns: E.valC
// Modifies: none
int performDump()
{
    return E.valC;
}

// Function: updateEregister
// Description:
// Params: stat -current pipeline status    icode -current instr
//         ifun -aditional info on instr    valC  -valC of current instr
//         valA -valA of current instr      valB  -valB of current instr
//         dstE -dstE of current instr      dstM  -dstM of current instr
//         srcA -srcA of current instr      srcB  -srcB of current instr
// Returns: void
// Modifies: E Register
void updateEregister(unsigned int stat, unsigned int icode, unsigned int ifun, 
                     unsigned int valC, unsigned int valA, unsigned int valB, unsigned int dstE, 
                     unsigned int dstM, unsigned int srcA, unsigned int srcB)
{
    E.stat = stat;
    E.icode = icode;
    E.ifun = ifun;
    E.valC = valC;
    E.valA = valA;
    E.valB = valB;
    E.dstE = dstE;
    E.dstM = dstM;
    E.srcA = srcA;
    E.srcB = srcB;
}

// Funstion clearEregister
// Description: Clears contents of the E register
// Params: none
// Returns: void
// Modifies: E
void clearEregister()
{
    E.stat = AOK;
    E.icode = NOP;
    clearBuffer((char *) &E, sizeof(E));
}
