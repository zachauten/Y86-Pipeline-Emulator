#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

// icode
#define NOP     1
#define HALT    0
#define RRMOVL  2
#define IRMOVL  3
#define RMMOVL  4
#define MRMOVL  5
#define OPL     6
#define JXX     7
#define CALL    8
#define RET     9
#define PUSHL   10
#define POPL    11
#define DUMP    12

#define RRFUN   0
#define CMOVLE  1
#define CMOVL   2
#define CMOVE   3
#define CMOVNE  4
#define CMOVGE  5
#define CMOVG   6

#define JMP     0
#define JLE     1
#define JL      2
#define JE      3
#define JNE     4
#define JGE     5
#define JG      6

// stat
#define AOK     1
#define HLT     2
#define ADR     3
#define INS     4

// opl
#define ADDL    0
#define SUBL    1
#define ANDL    2
#define XORL    3

#define FNONE   0

#endif
