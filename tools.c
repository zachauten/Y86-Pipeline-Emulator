#include <stdio.h>
#include "bool.h"
#include "tools.h"

/* 
   Replace the return 0 with the code to perform the required function.
   Do not change the function prototypes.
   Be sure to check for errors in input. 
   Document EACH function with a function header (description, parameters,
   return).
*/

/* getBits
 *      Returns only the specified portion of the int, represented by a bit range.
 * Params:   int low - [0,high], lower bound of bit range
 *           int high - [low,31], upper bound of bit range
 *           int source - bit pattern to mask
 * Returns:  int - source with requested bit pattern in low order bits
 */  
unsigned int getBits(int low, int high, unsigned int source)
{
    if(high < low || low < 0 || high > 31)
         return 0;
    source <<= (31-high);
    source >>= (31-(high-low));
    return source; 
} 

/* setBits
 *      Set the specified bits (to 1) in the provided int.
 * Params:    int low - [0,high], lower bound of bit range
 *            int high - [low,high] upper bound of bit range
 *            int source - bit pattern for setting bits
 *Returns:    int - source with specified bits set (1)
 */
unsigned int setBits(int low, int high, unsigned int source)
{
    if(high < low || low < 0 || high > 31)
        return source;
    unsigned int ones = getBits(low, high, -1);
    ones <<= low;
    return source | ones;
}

/* clearBits
 *      Clear the specified bits (to 0) in the provided int.
 * Params:    int low - [0,high], lower bound of bit range
 *            int high - [low,high], upper bound of bit range
 *            int source - the bit pattern for clearing bits
 * Returns:   int - source with specified bits cleared (0)
 */
unsigned int clearBits(int low, int high, unsigned int source)
{
     if(high < low || low < 0 || high > 31)
          return source;
     unsigned int zeros = getBits(low, high, -1);
     zeros = ~(zeros << low);
     return source & zeros;
}

/* assignOneBit
 *      Set a single bit of an int to 1 or 0.
 * Params:    int bitNumber - [0,31], the bit to set/clear
 *            int bitValue - (0 || 1), what to set the bit to
 *            int source - bit pattern in which to set a bit
 * Returns:   int - source with specified bit set/cleared
 */
unsigned int assignOneBit(int bitNumber, int bitValue, unsigned int source)
{
    if(bitValue == 1)
        return setBits(bitNumber, bitNumber, source);
    else if(bitValue == 0)
        return clearBits(bitNumber, bitNumber, source);
    else
        return source;
}
/* getByteNumber
 *      Get a single byte from an int.
 * Params:    int byteNumber - [0,3], byte to retrieve
 *            int source - bit pattern to retrieve a byte from
 * Returns:   char - single byte value from source
 */
unsigned char getByteNumber(int byteNo, unsigned int source)
{
    if(byteNo < 0 || byteNo > 3)
    {
         return 0;
    } 
    else
    {
        char temp = (char)(source >> (8*byteNo)) & 0xff;
        return temp;
    }
}

/*
 * putByteNumber
 *      Place specified byte into the provided bit pattern
 *      at the specified byte number location.
 * Params:    int byteNumber - [0,3], which byte to replace
 *            char byte - byte value to insert into source
 *            int source - bit pattern to insert the byte
 * Returns:   int - source with specifed byte replaced
 */
unsigned int putByteNumber(int byteNo, unsigned char byteValue, 
                           unsigned int source)
{
    if(byteNo < 0 || byteNo > 3)
        return source;
    source = clearBits(byteNo*8, (byteNo*8)+7, source);
    unsigned int temp = byteValue << byteNo*8;
    return source | temp;
}

/* buildWord
 *      Constructs an entire 32bit int bit pattern from the 4
 *      provided bytes.
 * Params:    char byte0 - least significant byte
 *            char byte1 - 2nd least significant byte
 *            char byte2 - 2nd most significant byte
 *            char byte3 - most significant byte
 * Returns:   int - the constructed int bit pattern
 */
unsigned int buildWord(unsigned char byte0, unsigned char byte1, 
                       unsigned char byte2, unsigned char byte3)
{
    unsigned int word = 0;
    word = putByteNumber(0, byte0, word);
    word = putByteNumber(1, byte1, word);
    word = putByteNumber(2, byte2, word);
    word = putByteNumber(3, byte3, word);
    return word;
}

/* isNegative
 *      Tests whether the provided bit pattern is negative when interpreted
 *      as a signed 32bit int.
 * Params:    int source - bit pattern to test
 * Returns:   bool (true || false), true if negative, otherwise false
 */
bool isNegative(unsigned int source)
{
    return getBits(31, 31, source) == 1;
}

/*
 * expandBits
 *      Inserts the individual bits of the provided bit pattern into
 *      an array of char with spaces between bytes.
 * Params:    int source - bit pattern to display
 *            char bits[] - contains a character representation of
 *                          binary in source
 * Returns:   void
 */
void expandBits(unsigned int source, char bits[36])
{
    int i;
    int k = 0;
    for(i = 34; i >= 0; i--)
    {
       if(i == 8 || i == 17 || i == 26)
       {
           bits[i] = ' ';
       }
       else 
       {
           int temp = getBits(k, k, source);
           if(temp == 0)
                bits[i] = '0';
           else
                bits[i] = '1';
           k++;
       }
    }
    bits[35] = 0;
}

/*
 * clearBuffer
 *      Sets size bytes at * buff in memory to 0.
 * Params:     char * buff - the byte address in memory
 *             int size - the number of elements to clear
 * Returns:    void
 */
void clearBuffer(char * buff, int size)
{
    int i;
    for(i = 0; i < size; i++)
    {
        *buff = 0;
        buff++;
    }
}
