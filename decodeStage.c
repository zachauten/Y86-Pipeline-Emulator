#include "bool.h"
#include "tools.h"
#include <stdio.h>
#include "instructions.h"
#include "status.h"
#include "forwarding.h"
#include "bubbling.h"
#include "decodeStage.h"
#include "executeStage.h"
#include "registers.h"

/* prototypes of Functions only called within this file */
static unsigned int getSrcA();
static unsigned int getSrcB();
static unsigned int getDstE();
static unsigned int getDstM();
static unsigned int selectFwdA(forwardType *forward, unsigned int d_srcA);
static unsigned int selectFwdB(forwardType *forward, unsigned int d_srcB);
static bool E_bubble(bubbleType bubble, unsigned int srcA, unsigned int srcB);
static void printDregister();

//D register holds the input for the decode stage.
//It is only accessible from this file. (static)

static dregister D;
/*
unsigned int d_srcA;
unsigned int d_srcB;
unsigned int d_dstE;
unsigned int d_dstM;
*/
// Function: getDregister
// Description: Returns a copy of the D register
// Params: none
// Returns: dregister
// Modifies: none
dregister getDregister()
{
    return D;
}

void decodeStage(forwardType forward, bubbleType *bubble)
{
    bubble->D_icode = D.icode;
    unsigned int d_srcA;
    unsigned int d_srcB;
    unsigned int d_dstE;
    unsigned int d_dstM;
    unsigned int srcA = getSrcA();
    unsigned int srcB = getSrcB();
    unsigned int valA = selectFwdA(&forward, srcA);
    unsigned int valB = selectFwdB(&forward, srcB);
    unsigned int dstE = getDstE();
    unsigned int dstM = getDstM();
    bubble->d_srcA = srcA;
    bubble->d_srcB = srcB;
    if(!E_bubble(*bubble, srcA, srcB))
    {
        updateEregister(D.stat, D.icode, D.ifun, D.valC, valA, valB, dstE, dstM, srcA, srcB);
    }
    else
    {
        updateEregister(1, NOP, 0, 0, 0, 0, RNONE, RNONE, RNONE, RNONE);
    }
}

// Function: E_bubble
// Description: Determines if the E Register needs bubbled
// Params: bubble- Bubbling struct
// Returns: bool- TRUE if needs to be bubblefd
// Modifies: none
bool E_bubble(bubbleType bubble, unsigned int srcA, unsigned int srcB)
{
    bool first = bubble.E_icode == JXX && !bubble.e_Cnd;
    bool sec = bubble.E_icode == MRMOVL || bubble.E_icode == POPL;
    bool third = bubble.E_dstM == srcA || bubble.E_dstM == srcB;
    return first || (sec && third);
}

// Function: getSrcA
// Description: decides srcA register
// Params: none
// Returns: register to use for SrcA
// Modifies: none
unsigned int getSrcA()
{
    switch(D.icode)
    {
        case RRMOVL:
        case RMMOVL:
        case OPL:
        case PUSHL:
            return D.rA;
        case POPL:
        case RET:
            return ESP;
    }
    return RNONE;
}

// Function: getSrcB
// Description: decides srcB register
// Params: none
// Returns: register to use for srcB
// Modifes: none
unsigned int getSrcB()
{
    switch(D.icode)
    {
        case OPL:
        case RMMOVL:
        case MRMOVL:
            return D.rB;
        case PUSHL:
        case POPL:
        case CALL:
        case RET:
            return ESP;
    }
    return RNONE;
}

// Function: getDstE
// Description: decides dstE register
// Params: none
// Returns: register to use for dstE
// Modifies: none
unsigned int getDstE()
{
    switch(D.icode)
    {
        case RRMOVL:
        case IRMOVL:
        case OPL:
            return D.rB;
        case PUSHL:
        case POPL:
        case CALL:
        case RET:
            return ESP;
    }
    return RNONE;
}

// Function: getDstM
// Description: decides dstM register
// Params: none
// Returns: register to use for dstM
// Modifies: none
unsigned int getDstM()
{
    if(D.icode == MRMOVL || D.icode == POPL)
    {
        return D.rA;
    }
    return RNONE;
}

// Function: selectFwdA
// Description: decides if data needs forwarded through pipeline
// Params: forwardType- *forward   uint- srcA    
// Returns: int -value to forward
// Modifies: none
unsigned int selectFwdA(forwardType *forward, unsigned int d_srcA)
{
    switch (D.icode)
    {
        case CALL:
        case JXX:
        return D.valP;
    }
    if(d_srcA == RNONE)
    {
        return 0;
    }
    if(d_srcA == forward->e_dstE)
        return forward->e_valE;
    if(d_srcA == forward->M_dstM)
        return forward->m_valM;
    if(d_srcA == forward->M_dstE)
        return forward->M_valE;
    if(d_srcA == forward->W_dstM)
        return forward->W_valM;
    if(d_srcA == forward->W_dstE)
        return forward->W_valE;
    return getRegister(d_srcA);
}
// Function: selectFwdB
// Description: decides if data needs forwarded through pipeline
// Params: forwardType- *forward   uint- srcB
// Returns: int -value to forward
// Modifies: none
unsigned int selectFwdB(forwardType *forward, unsigned int d_srcB)
{
    if(d_srcB == RNONE)
        return 0;
    if(d_srcB == forward->e_dstE)
        return forward->e_valE;
    if(d_srcB == forward->M_dstM)
        return forward->m_valM;
    if(d_srcB == forward->M_dstE)
        return forward->M_valE;
    if(d_srcB == forward->W_dstM)
        return forward->W_valM;
    if(d_srcB == forward->W_dstE)
        return forward->W_valE;
    return getRegister(d_srcB);
}

// Function: updateDRegister
// Description: Sets D Register to specified values
// Params: stat -current pipeline status    icode -current instr
//         ifun -additional info on instr   rA    -rA of current instr
//         rB   -rB of current instr        valC  -constant word of current instr
//         valP -valP of current instr
// Returns: none
// Modifies: D Register
void updateDRegister(unsigned int stat, unsigned int icode, unsigned int ifun, 
                     unsigned int rA, unsigned int rB, unsigned int valC, unsigned int valP)
{
    D.stat = stat;
    D.icode = icode;
    D.ifun = ifun;
    D.rA = rA;
    D.rB = rB;
    D.valC = valC;
    D.valP = valP;
}
        
// Function: clearDregister
// Description: Clears the D register
// Params: none
// Returns: void
// Modifies: D
void clearDregister()
{
    D.stat = AOK;
    D.icode = NOP;
    clearBuffer((char *) &D, sizeof(D));
}
