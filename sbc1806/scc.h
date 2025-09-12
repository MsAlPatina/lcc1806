//scc.h
//#include "stdint.h"
//char EOFchar;

void sccOUT(char x);
char sccIN();
char sccIN2();
unsigned int	getsRTS_asm();
//char *getbuffer(char*buffer,char EOFchar); //its like gets(*buffer); but faster, you may use ACK char, NACK char, CR or LF
char getINTregister();
void setINTregister(char x);


void includerofscc(){
asm(" include scc.inc\n");
}

