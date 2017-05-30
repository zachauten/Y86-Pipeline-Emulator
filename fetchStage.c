#include "bool.h"
#include "tools.h"
#include <stdio.h>
#include "memory.h"
#include "instructions.h"
#include "forwarding.h"
#include "bubbling.h"
#include "fetchStage.h"
#include "registers.h"

/* prototypes of Functions only called within this file */
static bool need_valC(int f_icode);
static bool need_regids(unsigned int f_icode);
static unsigned int selectPC(forwardType *forward);
static bool F_bubble();
static bool F_stall(bubbleType bubble);
static bool D_stall(bubbleType bubble);
static bool D_bubble(bubbleType bubble);
static void printFregister();

//F register holds the input for the fetch stage. 
//It is only accessible from this file. (static)
static fregister F;

// Function: getFregister
// Description: Returns a copy of the F register
// Params: none
// Returns: fregister
// Modifies: none
fregister getFregister()
{
    return F;
}

// Function: fetchStage
// Description:  
// Params:
// Returns: none
// Modifies:
void fetchStage(forwardType *forward, bubbleType bubble)
{
    unsigned int f_pc = selectPC(forward);
    bool imemError = FALSE;
    bool needValC, instr_valid, needs_regids;
    unsigned char instruction = getByte(f_pc, &imemError);
    unsigned int f_stat;
    unsigned int valC, valP;
    unsigned int rA = RNONE;
    unsigned int rB = RNONE;
    unsigned char byte1, byte2, byte3, byte4;
    unsigned int f_icode;
    unsigned int f_ifun;

    // F_Icode and F_Ifun.
    if(imemError)
    {
        f_icode = NOP;
        f_ifun = FNONE;
    }
    else
    {
        f_icode = getBits(4, 7, instruction);
        f_ifun = getBits(0, 3, instruction);
    }
    
    // Is Instruction Valid?
    instr_valid = instructionValid(&f_icode);

    // Determine Status Code for Fetched Instruction.
    if(imemError)
    {
        f_stat = ADR;
    } 
    else if(!instr_valid)
    {
        f_stat = INS;
    } 
    else if(f_icode == HALT)
    {
        f_stat = HLT;
    } 
    else
    {
        f_stat = AOK;
    }
    needs_regids = need_regids(f_icode);
    if(needs_regids)
    {
        unsigned char temp = getByte(f_pc+1, &imemError);
        rB = getBits(0, 3, temp);
        rA = getBits(4, 7, temp);
    }
    unsigned int start = valCStart(f_icode);
    if(need_valC(f_icode))
    {
            byte1 = getByte(f_pc+start, &imemError);
            byte2 = getByte(f_pc+1+start, &imemError);
            byte3 = getByte(f_pc+2+start, &imemError);
            byte4 = getByte(f_pc+3+start, &imemError);
            valC = buildWord(byte1, byte2, byte3, byte4);
    }
    else
    {
        valC = 0;
    }
    valP = incrementPC(f_icode, f_pc); 
    
    if(!F_stall(bubble))
    {
        if(predictPC(f_icode))
        {
            F.predPC = valC;
        }
        else
        {
            F.predPC = valP;
        }
    }

    if(!D_stall(bubble))
    {
        if(!D_bubble(bubble))
        {
            updateDRegister(f_stat, f_icode, f_ifun, rA, rB, valC, valP);
        } 
        else
        {
            updateDRegister(1, NOP, 0, RNONE, RNONE, 0, 0);
        }
    }
}

// Function: predictPC
// Description: determines whether PC needs predicted
// Params: int -icode of instruction
// Returns: bool -TRUE if instruction is a JXX or CALL
// Modifies: none
bool predictPC(unsigned int f_icode)
{
    switch(f_icode)
    {
        case JXX:
        case CALL:
        return TRUE;
        default:
        return FALSE;
    }
}

// Function: incrementPC
// Description: increments PC based on instruction 
// Params: icode
// Returns: int -incremented PC
// Modifies: none
int incrementPC(unsigned int f_icode, unsigned int f_pc)
{
    switch(f_icode)
    {
        case IRMOVL:
        case RMMOVL:
        case MRMOVL:
            return f_pc + 6;
        case DUMP:
        case JXX:
        case CALL:
            return f_pc + 5;
        case OPL:
        case RRMOVL:
        case PUSHL:
        case POPL:
            return f_pc + 2;
    }
    return f_pc + 1;
}

// Function: valCStart
// Description: Determines the starting byte of valC
// Params: icode of instruction
// Returns: int- valC starting byte
// Modifies: none
int valCStart(unsigned int f_icode)
{
    if(f_icode == IRMOVL || f_icode == RMMOVL || f_icode == MRMOVL)
        return 2;
    return 1;
}

// Function: need_regids
// Description: Returns whether the instruction needs a register
// Params: f_icode- Current instruction
// Returns: bools
// Modifies: none
bool need_regids(unsigned int f_icode)
{
    return (f_icode == RRMOVL || f_icode == OPL || f_icode == PUSHL || f_icode == POPL || 
        f_icode == IRMOVL || f_icode == RMMOVL || f_icode == MRMOVL);
}

// Function: instructionValid
// Description: Returns whether the instruction is valid
// Params: f_icode- Current instruction
// Returns: bool
// Modifies: none
bool instructionValid(unsigned int * f_icode)
{
    switch(*f_icode)
    {
        case NOP:
        case HALT:
        case RRMOVL:
        case IRMOVL:
        case RMMOVL:
        case MRMOVL:
        case OPL:
        case JXX:
        case CALL:
        case RET:
        case PUSHL:
        case POPL:
        case DUMP:
            return TRUE;
    }
    return FALSE;
}

// Function: need_valC
// Description: Determines whether fetched instruction requires
//              a constant word
// Params: f_icode- Current instruction
// Returns: bool
// Modifies: none
bool need_valC(int f_icode)
{
    return (f_icode == IRMOVL || f_icode == DUMP || f_icode == RMMOVL || 
            f_icode == MRMOVL || f_icode == JXX || f_icode == CALL);
}

// Function: selectPC
// Description: Determines PC based on forwarded values
// Params: forward- Forwarding struct
// Returns: int- Selected PC value
// Modifies: none
unsigned int selectPC(forwardType *forward)
{
    if(forward->M_icode == JXX && !forward->M_Cnd)
    {
        return forward->M_valA;
    }
    if(forward->W_icode == RET)
    {
        return forward->W_valM;
    }
    return F.predPC;
}

// Function: F_bubble
// Description: Determines if F Register needs bubbled
// Params: none
// Returns: FALSE- F Register should not be bubbled
// Modifies: none
bool F_bubble()
{
    return FALSE;
}

// Function: F_stall
// Description: Determines if F Register needs stalled
// Params: bubble- Bubbling struct
// Returns: bool- TRUE if needs to be stalled
// Modifies: none
bool F_stall(bubbleType bubble){
    return ((bubble.E_icode == MRMOVL || bubble.E_icode == POPL) &&
            (bubble.E_dstM == bubble.d_srcA || bubble.E_dstM == bubble.d_srcB)) ||
            (bubble.D_icode == RET || bubble.D_icode == RET || bubble.M_icode == RET);
}

// Function: D_stall
// Description: Determines if D Register needs stalled
// Params: bubble- Bubbling struct
// Returns: bool- TRUE if needs to be stalled
// Modifies: none
bool D_stall(bubbleType bubble)
{
    return (bubble.E_icode == MRMOVL || bubble.E_icode == POPL) &&
           (bubble.E_dstM == bubble.d_srcA || 
            bubble.E_dstM == bubble.d_srcB);
}

// Function: D_bubble
// Description: Determines if D Register needs bubbled
// Params: bubble- Bubbling struct
// Returns: bool- TRUE if needs to be bubbled
// Modifies: none
bool D_bubble(bubbleType bubble)
{
    return (bubble.E_icode == JXX && !(bubble.e_Cnd)) ||
           (!((bubble.E_icode == MRMOVL || bubble.E_icode == POPL) &&
           (bubble.E_dstM == bubble.d_srcA || bubble.E_dstM == bubble.d_srcB)) &&
           (bubble.D_icode == RET || bubble.E_icode == RET || bubble.M_icode == RET));
}

// Function: clearFregister
// Description: Clears contents of the F register
// Params: none
// Returns: none
// Modifies: F 
void clearFregister()
{
    clearBuffer((char *) &F, sizeof(F));
}
