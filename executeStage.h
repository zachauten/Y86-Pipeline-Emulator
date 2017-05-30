#ifndef EXECUTESTAGE_H
#define EXECUTESTAGE_H

typedef struct
{
    unsigned int stat;
    unsigned int icode;
    unsigned int ifun;
    unsigned int valC;
    unsigned int valA;
    unsigned int valB;
    unsigned int dstE;
    unsigned int dstM;
    unsigned int srcA;
    unsigned int srcB;
} eregister;

//prototypes for functions called from files other than fetchStage
eregister getEregister();
void updateERegister(unsigned int stat, unsigned int icode, unsigned int ifun, 
                     unsigned int valC, unsigned int valA, unsigned int valB, unsigned int dstE, 
                     unsigned int dstM, unsigned int srcA, unsigned int srcB);

void executeStage(forwardType *forward, statusType *status, bubbleType *bubble);

void clearEregister();
#endif
