#include <stdio.h>
#include <string.h>
#include "bool.h"
#include "registers.h"

static unsigned int registers[REGSIZE];
static unsigned int CC;

// Function: getRegister
// Description: Returns a specific register's content
// Params: regNum- the register to retrieve data
// Returns: Data from registers[regNum]
// Modifies: none 
unsigned int getRegister(int regNum)
{
    if(regNum < 0 || regNum > (REGSIZE-1))
        return 0;
    else
        return registers[regNum];
}

// Function: setRegister
// Description: Sets a certain register to a specific value
// Params: regNum- the register to modify   regValue- value to set
// Returns: none
// Modifies: registers[regNum]
void setRegister(int regNum, unsigned int regValue)
{
    if(regNum >= 0 && regNum < REGSIZE)
    {
        registers[regNum] = regValue;
    }
}

// Function: clearRegisters
// Description: Clears register data
// Params: none
// Returns: none
// Modifies: registers 
void clearRegisters()
{
    memset(registers,0,sizeof(registers));
}

// Function: setCC
// Description: sets the bit number of CC specified by the bitNumber
//              parameter to the value in the value parameter (should be 0 or 1)
// Params: bitNumber- bit to set in CC   value- set bit to 0 or 1
// Returns: none
// Modifies: specific bitNumber in CC
void setCC(unsigned int bitNumber, unsigned int value)
{
    if(value != 0 && value != 1) return;
    CC = assignOneBit(bitNumber, value, CC);
}

// Function: getCC
// Description: returns the value of the bitNumber bit in CC
// Params: bitNumber- specific bit in CC
// Returns: value of the bitNumber bit in CC
// Modifies: none
unsigned int getCC(unsigned int bitNumber)
{
    return getBits(bitNumber, bitNumber, CC);
}




