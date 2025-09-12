//header file of the GPU API

void write_GPU(char address,char data);
void gpu_setPortAddress(char port, unsigned long int address, char increment);
void initScreen();
void gpu_writeCharAt(char c);
void gpu_writeStringAt(char *str, char x, char y,char color);
void gpu_clear_display();
char read_GPU(char address);
void gpu_clearDisplay();
#include "GPU.c"
void includerGPU(){
    asm("\tinclude GPU.inc\n");
}

