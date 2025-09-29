//syscall like stuff

#define SYSCALLADDRESS 0x7F00 //where in memory are function pointers at

/*
void printf(char *pptr,...){

    asm("   SCAL R4,0x5710\n");//this addresses must be adjusted by hand. check the numbers at 0x7F00

}

void mputc(char c){
    asm("   SCAL R4,0x15AC\n");
}
char mgetc(){
    asm("   SCAL R4,0x1E13\n");
}

char getKeyboard(){
    asm("   SCAL R4,0x1D9A\n");
}
    */



/*
void sccOUT(){
    asm("   SCAL R4,0x13E6\n");
}

char sccIN(){
    asm("   SCAL R4,0x1404\n");

}
*/

void printf(char *pptr,...){

    asm("   RLDI R8, 0x7F00\n");//go fetch our address
    asm("   LDA R8\n");//get the high byte
    asm("   PHI R15\n");//put high order of R15
    asm("   LDN R8\n");//get the low byte
    asm("   PLO R15\n");//put low order of R15
    asm("   Ccall *R15\n");//use the address stored in R15

}

void mputc(char c){
    asm("   RLDI R8, 0x7F02\n");//go fetch our address
    asm("   LDA R8\n");//get the high byte
    asm("   PHI R15\n");//put high order of R15
    asm("   LDN R8\n");//get the low byte
    asm("   PLO R15\n");//put low order of R15
    asm("   Ccall *R15\n");//use the address stored in R15
}
char mgetc(){
    asm("   RLDI R8, 0x7F04\n");//go fetch our address
    asm("   LDA R8\n");//get the high byte
    asm("   PHI R15\n");//put high order of R15
    asm("   LDN R8\n");//get the low byte
    asm("   PLO R15\n");//put low order of R15
    asm("   Ccall *R15\n");//use the address stored in R15
}

char getKeyboard(){
    asm("   RLDI R8, 0x7F06\n");//go fetch our address
    asm("   LDA R8\n");//get the high byte
    asm("   PHI R15\n");//put high order of R15
    asm("   LDN R8\n");//get the low byte
    asm("   PLO R15\n");//put low order of R15
    asm("   Ccall *R15\n");//use the address stored in R15
}

void sccOUT(){
    asm("   RLDI R8, 0x7F08\n");//go fetch our address
    asm("   LDA R8\n");//get the high byte
    asm("   PHI R15\n");//put high order of R15
    asm("   LDN R8\n");//get the low byte
    asm("   PLO R15\n");//put low order of R15
    asm("   Ccall *R15\n");//use the address stored in R15
}

char sccIN(){
    asm("   RLDI R8, 0x7F0A\n");//go fetch our address
    asm("   LDA R8\n");//get the high byte
    asm("   PHI R15\n");//put high order of R15
    asm("   LDN R8\n");//get the low byte
    asm("   PLO R15\n");//put low order of R15
    asm("   Ccall *R15\n");//use the address stored in R15

}


//from SYSCALL from bios_gpu.c
/*
    asm(" SET SAVELOC,$\n");
	asm("	ORG 0x7F00\n");
	asm("	dw _printf\n"); --
    asm("   dw _mputc\n");--
    asm("   dw _mgetc\n");--
    asm("   dw _getKeyboard\n");--
    asm("   dw _sccOUT\n");
    asm("   dw _sccIN\n");
    asm(" org SAVELOC\n");
*/