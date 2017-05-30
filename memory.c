#include <stdio.h>
#include "bool.h"
#include "memory.h"
#include "tools.h"

unsigned int fetch(int address, bool *memError);
void store(int address, unsigned int value, bool *memError);

static unsigned int memory[MEMSIZE];

// Function: fetch
// Description: reads a word of the primary memory
// Params: address- a word address
// Returns: an unsigned int (addressedWord) containing the 32-bit contents
// of the cell at the address
// Modifies: none 
unsigned int fetch(int address, bool * memError)
{
    unsigned int addressedWord;
    if(address < 0 || address > (MEMSIZE - 1))
    {
        *memError = TRUE;
        addressedWord = 0;
        return addressedWord;
    }
    else
    {
        addressedWord = memory[address];
        *memError = FALSE;
        return addressedWord;
    }
}

// Function: store
// Description: writes a word to the primary memory
// Params: address- a word address   value- the 32-bit value to be stored
// memError- set to TRUE or FALSE depending on the address' validity
// Returns: none
// Modifies: word address cell value 
void store(int address, unsigned int value, bool * memError)
{
    if(address < 0 || address > (MEMSIZE - 1))
    {
        *memError = TRUE;
    }
    else
    {
        memory[address] = value;
        *memError = FALSE;
    }
}

// Function: getByte
// Description: converts byte address to word address to fetch byte
// Params: address- a byte address   memError- set to TRUE or FALSE
// depeding on the addresss' validity
// Returns: selected byte from address
// Modifies: none   
unsigned char getByte(int address, bool *memError)
{
    unsigned char addressedByte;
    if(address < 0 || address > ((MEMSIZE * 4) - 1))
    {
        *memError = TRUE;
        return 0;
    } 
    else
    {
        *memError = FALSE;
        return getByteNumber(address % 4, fetch(address>>2, memError));
    }
}

// Function: putByte
// Description: converts byte address to word address and stores byte
// Params: address- a byte address   value- byte to be stored
// memError- set to TRUE or FALSE depending on the address' validity
// Returns: none
// Modifies: selected byte from address
void putByte(int address, unsigned char value, bool *memError)
{
    if(address < 0 || address > ((MEMSIZE * 4) - 1))
    {
        *memError = TRUE;
    } 
    else
    {
       *memError = FALSE;
       unsigned int temp = putByteNumber(address % 4, value, fetch(address / 4, memError));
       store(address / 4, temp, memError);
    }
}

// Function: clearMemory
// Description: initializes (zeros) the contents of memory
// Params: none
// Returns: none
// Modifies: contents of memory
void clearMemory()
{
    int i;
    for(i = 0; i < MEMSIZE; i++)
    {
        memory[i] = 0;
    }
}

// Function: getWord
// Description: returns 32-bit value from specified address
// Params: address- a byte address   memError- set to TRUE or FALSE
//         depending on address' validity
// Returns: 32-bit value at specified address
// Modifies: none
unsigned int getWord(int address, bool *memError)
{
    if(address % 4 != 0 || address > ((MEMSIZE * 4) - 1))
    {
        *memError = TRUE;
        return 0;
    }
    *memError = FALSE;
    return fetch(address >> 2, memError);
}

// Function: putWord
// Description: sets a 32-bit value in specified address
// Params: address- a byte address   value- 32-bit value to set
//         memError- set to TRUE or FALSE depending on address' validity
// Returns: none
// Modifies: 32-bit value at specified address
void putWord(int address, unsigned int value, bool * memError)
{
    if(address % 4 != 0 || address > ((MEMSIZE * 4) - 1))
    {
        *memError = TRUE;
    } 
    else
    {
        *memError = FALSE;
        store((address >> 4), value, memError);
    }
}
