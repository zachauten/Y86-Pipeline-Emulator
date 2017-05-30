#include <stdio.h>
#include "bool.h"
#include <string.h>
#include "loader.h"
#include "memory.h"

// Function: validFileName
// Description: determines correctness of filename (.yo)
// Params: a string (char *) representing name of a file
// Returns: TRUE if filename is correct, FALSE otherwise
// Modifies: none
bool validFileName(char *name)
{
    int len = strlen(name);
    int i = len-3;
    char *ptr = &name[i];
    if(*ptr == '.')
    {
        ptr++;
        if(*ptr == 'y')
        {
            ptr++;
            if(*ptr == 'o')
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}


// Function: grabAddress
// Description: returns the address in a specific line (data record)
// Params: line to retrieve address from
// Returns: address from specified line
// Modifies: none
unsigned int grabAddress(char *data)
{
    unsigned int address = strtol(data+2, NULL, 16);
    return address;
}

// Function: discardRest
// Description: If the fgets didn't read a newline than the entire line
//              wasn't read by the last call to fgets. This is because the
//              line in the file is larer than our 80 character limit.
//              discardRest looks for the newline character in the input
//              line. If it's not in the line, the function reads (and discards)
//              characters from the file until newline is read.
// Params: the most recently read line and pointer to the file being read
// Returns: none
// Modifies: file pointer positioned at beginning of next line of input file
void discardRest(char *line, FILE *fp)
{
    int len = strlen(line);
    char c;
    if(line[len-1] != 10)
    {
        while(!feof(fp) && ((c = fgetc(fp)) != '\n'));
    }
}

// Function: isAddress
// Description: determines whether the data record contains an address
// Params: data record to verify address
// Returns: TRUE or FALSE depending if an address is present
// Modifies: none
bool isAddress(char *line)
{
    if(((*(line + 2)) == 48) && ((*(line+3)) == 120) && (isxdigit(*(line+4))) &&
        (isxdigit(*(line+5))) && (isxdigit(*(line+6))) && ((*(line+7)) == 58))
        return TRUE;
    return FALSE;
}

// Function: isData
// Description: determines whether the data record contains data to
//              be stored and computes how many bytes
// Params: data record to check for data to be stored
// Returns: number of bytes to be stored
// Modifies: none

int isData(char *line)
{
    int i = 9;
    int count = 0;
    for(i;i<21;i++)
        {
            if(isxdigit(*(line+i)))
            {
                count++;
            }
            if((*(line+i)) == ' ')
            {
                if(!isSpaces(line, i, 20))
                {
                    return -1;
                }
                break;
            }
        }
    if(count%2 == 0)
    {
        return count/2;
    }
    else
    {
        return -1;
    }
}

// Function: isSpaces
// Description: checks whether spaces are present from the startingIndex
//              to the endingIndex of a data record
// Params: startingIndex to begin, endingIndex to stop, and data record to check
// Returns: TRUE or FALSE depending whether spaces are present between startingIndex
//          and endingIndex
// Modifies: none 
bool isSpaces(char *line, int startingIndex, int endingIndex)
{
    int i = startingIndex;
    for(i; i <= endingIndex; i++)
    {
        if(line[i] != ' ')
            return FALSE;
    }
    return TRUE;
}

// Function: allSpaces
// Description: checks whether spaces are in the correct positions
// Params: an input line
// Returns: TRUE or FALSE depending on correct space location in input line
// Modifies: none
bool allSpaces(char *line)
{
    if(isSpaces(line,0,1) && (*(line+8)) == ' ' && (*(line+21)) == ' ')
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// Function: checkHex
// Description: checks whether there are hex characters from the startingIndex to
//              the endingIndex of a data record
// Params: startingIndex to begin, endingIndex to stop, and data record to check hex
//         characters
// Returns: TRUE or FALSE depending whether hex characters are present between startingIndex
//          and endingIndex
// Modifies: none
bool checkHex(char *line, int startingIndex, int endingIndex)
{
    int i = startingIndex;
    for(i; i <= endingIndex; i++)
    {
        if(!isxdigit(*(line+i)))
        {
            return FALSE;
        }
    }
    return TRUE;
}

// Function: grabDataByte
// Description: gets the value of a data byte from specified position
// Params: data record and startingIndex from the record
// Returns: data byte from the startingIndex 
// Modifies: none
unsigned int grabDataByte(char *line, int startingIndex)
{
    char byte[2];
    switch(startingIndex)
    {
        case 1:
            byte[0] = *(line+9);
            byte[1] = *(line+10);
            break;
        case 2:
            byte[0] = *(line+11);
            byte[1] = *(line+12);
            break;
        case 3:
            byte[0] = *(line+13);
            byte[1] = *(line+14);
            break;
        case 4:
            byte[0] = *(line+15);
            byte[1] = *(line+16);
            break;
        case 5:
            byte[0] = *(line+17);
            byte[1] = *(line+18);
            break;
        case 6:
            byte[0] = *(line+19);
            byte[1] = *(line+20);
            break;
    }
    return strtol(byte, NULL, 16);                                    
}

// Function: checkLine
// Description: checks for errors in the input line
// Params: an input line
// Returns: TRUE or FALSE depending on correctness of input line
// Modifies: none
bool checkLine(char *line, unsigned int lastAddress)
{
    if(isSpaces(line, 0, 21))
        return TRUE;
    if((allSpaces(line)) && (isAddress(line)) && (checkAddress(line, lastAddress)) &&
        (isData(line) >= 0))
        return TRUE;
    return FALSE;
}
// Function: checkAddress
// Description: checkes whether the address is greater than the last address
//              to which data would be stored
// Params: a line and the last address
// Returns: TRUE or FALSE depending on whether the address is greater than the last
// Modifies: none
bool checkAddress(char *line, unsigned int lastAddress)
{
    unsigned int tester = grabAddress(line);
    if(tester >= (((int) lastAddress) + 1))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// Function: load
// Description: loads a YESS program into memory making sure
//              the input file is free of errors
// Params: arguments passed to main and a file
// Returns: TRUE or FALSE depending on load status
// Modifies: memory 
bool load(int parameters, char * file[])
{
    char myString[80];
    unsigned int lastAddress = -1;
    int lineCounter = 0;
    unsigned int numberBytes, lineAddress;
    int addressCounter, i;
    bool memError;
    if(parameters == 2 && validFileName(file[1]))
    {
        FILE *fp = fopen(file[1], "r");
        if(fp != NULL) 
        {
            while(!feof (fp))
            {
                if(fgets(myString, sizeof(myString), fp) != NULL)
                {
                    if(checkLine(myString, lastAddress))
                    {
                        numberBytes = isData(myString);
                        lineAddress = grabAddress(myString);
                        addressCounter = lineAddress;
                        lineCounter++;
                        if(numberBytes > 0)
                        {
                            lastAddress = (lineAddress + (numberBytes - 1));
                        }
                        for(i = 1; i <= numberBytes; i++)
                        {
                            putByte(addressCounter, (unsigned char) grabDataByte(myString, i), &memError);
                            addressCounter += 1;
                        }
                        discardRest(myString, fp);
                     }
                     else
                     {
                         printf("%s %d\n", "Error on line", (lineCounter + 1)); 
                         printf("%s\n", myString);
                         return FALSE;
                     }
                }
            }
            fclose(fp);
            return TRUE;
        }
    }
    printf("%s", "Can't open file!\n");
    return FALSE;
}   
