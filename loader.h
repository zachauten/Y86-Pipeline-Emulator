#ifndef LOADER_H
#define LOADER_H
bool validFileName(char *name);
unsigned int grabAddress(char *data);
void discardRest(char *line, FILE *fp);
bool isAddress(char *line);
bool isData(char *line);
bool isSpaces(char *line, int startingIndex, int endingIndex);
bool allSpaces(char * line);
bool checkHex(char *line, int startingIndex, int endingIndex);
unsigned int grabDataByte(char *line, int startingIndex);
bool checkLine(char * line, unsigned int lastAddress);
bool checkAddress(char * line, unsigned int lastAddress);
bool load(int parameters, char * file[]);
#endif
