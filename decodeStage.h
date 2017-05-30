#ifndef DECODESTAGE_H
#define DECODESTAGE_H
typedef struct
{
    unsigned int stat;
    unsigned int icode;
    unsigned int ifun;
    unsigned int rA;
    unsigned int rB;
    unsigned int valC;
    unsigned int valP;
} dregister;

//prototypes for functions called from files other than decodeStage
dregister getDregister();
void decodeStage(forwardType forward, bubbleType *bubble);
void updateDRegister(unsigned int stat, unsigned int icode, unsigned int ifun, 
                     unsigned int rA, unsigned int rB, unsigned int valC, unsigned int valP);
void clearDregister();
#endif
