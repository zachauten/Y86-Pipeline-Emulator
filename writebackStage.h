#ifndef WRITEBACKSTAGE_H
#define WRITEBACKSTAGE_H
typedef struct
{
    unsigned int stat;
    unsigned int icode;
    unsigned int valE;
    unsigned int valM;
    unsigned int dstE;
    unsigned int dstM;
} wregister;

wregister getWregister();
bool writebackStage(forwardType *forward, statusType *status);
void updateWregister(unsigned int stat, unsigned int icode, unsigned int valE,
                     unsigned int valM, unsigned int dstE, unsigned int dstM);

void clearWregister();
#endif
