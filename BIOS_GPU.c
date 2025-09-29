//a version of the bios with a GPU
//BIOS.c


//wine lcc -target=xr18CX "-Wa -D STACKLOC=0xEEEE" "-Wa -D NOFILLBSS=1 " "-Wa -D DATALOC=0xC000" "-Wa -D CPUSPEED=5000000" BIOS.c -o BIOS.hex "-Wf -volatile -cpu1805" "-DSCC=2" "-DDEBUG=0"

//srec_cat bios_gpu.hex -Intel -offset 0x0000 bios_gpu.hex -Intel -offset 0x8000 bios_gpu.hex -Intel -offset 0x10000 bios_gpu.hex -Intel -offset 0x18000 -crop 0x0000 0x20000 -o output.hex -Intel -contradictory-bytes warning


//output é HEX não decimal

//minipro -p AM29F010@PLCC32 -w output.hex
//set "-DSCC=1" to "-DSCC=0" to set it to bitbang Serial
//DATALOC moved from 0x8000 to 0xC000 to have 16K of ram free for bootloading
//stack went from 0xFFFF to EEEE to allow BIOS (reserved memory) bytes
//we have as such 12 014Bytes of free memory


#include <stdlib.h>
#include <olduino.h>
#include <rand.h>
#include <PCF8584_DRIVER.h>
#include <GPU.h>
#include "PCF_DRIV.c"
//#include <stdint.h>
void sccDEFAULT();
//#include "SPI.h"
#define NULL 0x00
//#include <1806.h>
#include <EF.h>
#ifndef ULONG_MAX
#define	ULONG_MAX	((unsigned long)(~0L))		
#endif

#define ERANGE ((unsigned long)(~0L))

#if(SCC == 0)
//#define putc(x) putcser(x)
unsigned char getcser();
#define getc() getcser()
void putcser(unsigned char);

#endif


#if((SCC == 2)&(DEBUG == 0))

#include "scc.h"
//#define putc(x) sccOUT(x);
#define putc(x) mputc(x)
//#define getc() sccIN()
//#define getc() getKeyboard()
#define getc() mgetc()
void sccOUT(char);
char sccIN();

#endif

#if((SCC == 2)&(DEBUG == 1))

//#include "sccDEBUGGED.h"
//#define putc(x) sccOUT(x);
//#define getc() sccIN()
//void sccOUT(char);
//char sccIN();

#endif



#if(DEBUG == 1)
#error DEBUG MODE IS ON
#endif
//#define EOT 4
//#define REGADDR (unsigned int *)0xFF00 //where the registers are stored on reset
//#define CODEADDR (unsigned char *)0x8000 //address where code is loaded
//const int maxInputLength = 200; //max ammount of data you can send in one line
//const int maxFileSize = 8192; //max of 8Kbytes per file size
char getINint();
void printShape(int color, int x, int y, int length, int height);
void calculator(char* varA, char* varB,char* operation);
void inputprompt(char* cin,char* cin2);
void outputprompt(char* cin,char* cin2,int buffer);
void infodisplay();
void hexdumper(int start,int end);
void hexdump(unsigned char *start, int span);
void mem_view(unsigned int* startAddress, int length);
void printsn(float k);
void floatToScientific(float num);
void mem_view(unsigned int* startAddress, int length);
unsigned long int checksum(char *data,unsigned int size);
long double expression(char* string, char** lastPosition);
void IntToScientific(int num);
void UIntToScientific(unsigned int num);
void switchbank(char banknumber);
void ADCPRINTVALUE(int channel,char relay);
void RTCPOKE();
void BIOSMENU();
unsigned char write_data(unsigned long int address,unsigned char *data,unsigned int size);
char*   getsSized(char *s,unsigned int size);
char SERIALMODE;

//used for IO, to set or reset a bit, you XOR the bit you want to set with the reg and then output the reg
//its safer, so it doesnt interfear with other bits
/* global vars breaking things again
//char OREG1;
//char IOREG2A;
//char IOREG2B;
char IOREG2C;

char IOREG3A;
char IOREG3B;
char IOREG3C;

char IOREG4A;
char IOREG4B;
char IOREG4C;

char IOREG5A;
char IOREG5B;
char IOREG5C;
*/
//long int tester;
//long int tester;

double ANS; //value for ans
char gpiocount;
int usermem;
char *gets(char *);
void Sky();
//char memorydisplay; //to enable or disable the memory viewer
//	char  EF1func=(char )0xfff0;
//	char  EF2func=(char )0xfff1;
//	char  EF3func=(char )0xfff2;
//	char  EF4func=(char )0xfff3;
//	char  ADCIO=(char )0xfff4;
//	char  baudrate=(char )0xfff5;
	int progmemorysize;



// Screen dimensions
#define VGA_WIDTH 128
#define VGA_HEIGHT 64

typedef enum {
    OUTPUT_SERIAL,
    OUTPUT_VGA,
    OUTPUT_BOTH
} output_mode_t;

static output_mode_t current_output = OUTPUT_SERIAL;
static uint8_t vga_x = 0;
static uint8_t vga_y = 0;
static uint8_t vga_color = 0x0F; // Default white on black
static uint8_t is_escape_code = false;

void set_output_mode(output_mode_t mode) {
    current_output = mode;
}

void set_vga_cursor(uint8_t x, uint8_t y) {
    if (x < VGA_WIDTH && y < VGA_HEIGHT) {
        vga_x = x;
        vga_y = y;
    }
}

void set_vga_color(uint8_t color) {
    vga_color = color;
}

void vga_scroll() {

    vga_x = 0;
    vga_y = 0;
}

void mputc(char c) {
    static int escape = 0;
    static int ansi_state = 0;
    static int ansi_params[4] = {0};
    static int param_index = 0;
    static int current_param = 0;
    int i;
    int code;
    
    char arr[2] = {0x00, 0x00};
    
    // Always output to serial if enabled
    //sccOUT(c);
    
    // Handle ANSI escape sequences
    if(!TEF1()){
        //serial mode
        sccOUT(c);
        write_GPU(0x09, 0x10); //turn VGA OFF

    }else{
        //vga mode
    if (escape == 1) {
        if (c == '[') {
            escape = 2;  // CSI sequence started
            ansi_state = 0;
            param_index = 0;
            current_param = 0;
            for (i = 0; i < 4; i++) ansi_params[i] = 0;
            return;  // Don't process this character further
        } else {
            escape = 0;  // Not a CSI sequence, abort
        }
    } else if (escape == 2) {
        if (c >= '0' && c <= '9') {
            current_param = current_param * 10 + (c - '0');
        } else if (c == ';') {
            if (param_index < 3) {
                ansi_params[param_index++] = current_param;
                current_param = 0;
            }
        } else if (c == 'm') {
            // Process SGR (Select Graphic Rendition) command
            ansi_params[param_index] = current_param;
            
            for (i = 0; i <= param_index; i++) {
                code = ansi_params[i];
                
                // Handle color codes
                if (code >= 30 && code <= 37) {
                    // Foreground colors
                    vga_color = (vga_color & 0xF0) | (code - 30);
                } else if (code == 39) {
                    // Default foreground
                    vga_color = (vga_color & 0xF0) | 0x01;
                } else if (code >= 40 && code <= 47) {
                    // Background colors
                    vga_color = (vga_color & 0x0F) | ((code - 40) << 4);
                } else if (code == 49) {
                    // Default background
                    vga_color = 0x00 |(vga_color & 0x0F);//(vga_color & 0x0F) | (0x00 << 4);
                } else if (code == 0) {
                    // Reset all attributes
                    vga_color = 0x01;
                }
            }
            
            escape = 0;
            return;
        } else if (c == 'J') {
            // Process erase display command
            ansi_params[param_index] = current_param;
            
            // Only handle parameter 2 (clear entire screen) for now
            if (ansi_params[0] == 2) {
                // Clear the screen by resetting cursor and filling with spaces
                vga_x = 0;
                vga_y = 0;
                //gpu_clear_display();
                gpu_setPortAddress(0, 0, 1);
                gpu_clearDisplay();
            }
            
            escape = 0;
            return;
        } else {
            // Unknown ANSI sequence, abort
            escape = 0;
        }
    }
    
    if (c == 0x1B) {  // ESC character
        escape = 1;
        return;  // Don't process this character further
    }
    
    // Handle VGA output if enabled
    switch (c) {
        case '\n':
            vga_x = 0;
            vga_y++;
            break;
        case '\r':
            vga_x = 0;
            break;
        case '\t':
            vga_x = (vga_x + 8) & ~7; // Tab to next 8-column boundary
            if (vga_x >= VGA_WIDTH) {
                vga_x = 0;
                vga_y++;
            }
            break;
        case '\b':
            if(vga_x > 0){
                vga_x--;
            }
            break;
        default:
            if (!escape) {  // Only print if we're not in an escape sequence
                arr[0] = c;
                asm("   SEQ\n");
                gpu_writeStringAt(arr, vga_x, vga_y, vga_color);
                asm("   REQ\n");
                vga_x++;
            }
            break;
    }
    
    // Handle line wrapping and screen edge
    if (vga_x >= VGA_WIDTH) {
        vga_x = 0;
        vga_y++;
    }
    
    if (vga_y >= VGA_HEIGHT) {
        vga_scroll();
    }
    }
}



unsigned int atoui(register char* inputString)
{
    unsigned int numResult = 0;
    while(*inputString >= '0' && *inputString <= '9') {
        numResult *= 10;
        numResult += *inputString - 48;
        inputString++;
    }

    return numResult;
}
 char i2c_readdata(char address){
    char datareaded=0x00;

    address<<=1;//shift left LSB is now 0
    address|=0x01;//or an 0x01 so it becomes a read op
    out(1,0x00);
    out(5,address);
    out(1,0x01);
    out(5,0xC5);
    /*while(inp(5)&0x80){
        asm("    NOP\n");
    }*/
    
    out(1,0x01);    //send NACK on the end of reception.
    out(5,0x40);
            
    out(1,0x00);
    datareaded=inp(5);  //this is a dummy read!!!!
    
    /* out(1,0x01);
    while(inp(7)&0x80){
        asm("    NOP\n");
    }                       //waiting to finish reception. */
    
    out(1,0x01);    //STOP
    out(5,0xC3);
    
    out(1,0x00);
    datareaded=inp(5);      //reading the actual received byte.
    
    return datareaded;
} 
#define keyboard 0x5F
char getKeyboard(){
    char c;
    c=0x00;
	while(c==0x00){
        c=i2c_readdata(keyboard);
        switch(c){
            case 0xA9:
            //FN+V to toggle VGA
                write_GPU(0x09, 0x11); //VGA
                c=0x00;//to stay in loop
                break;
            case 0xA8:
            //FN-C to toggle composite
                write_GPU(0x09, 0x12); //composite NTSC
                c=0x00;//to stay in loop
                break;
            
            

            default:
                break;


        
        }
    }




    //putc(c);
    return c;


}


char mgetc(){

    if(!TEF1()){

        return sccIN();

    }else{
        return getKeyboard();
    }

}

char*   gets(char *s)
{

    char ch,*p;
    usermem = 0;
    p=s;
	
    while ((ch=getc()) != '\r') { //replaced \r \n with 
	
        //putc(0x13); //send DC3, so XOFF, stop TX decimal 19
        if(ch == 0x08) { //\b is used but i go with 0x08
            *s = '\0';
		  // *s--;
            if(s > p) {
                s--;    //added backspace compatability , so no more pressing enter to undo your error
            }
        } else {
            *s++ = ch;
            usermem++; //increment the memory pointer
        }
        //printf("%c",ch);
        putc(ch);
        //putc(0x11); //send DC1, so XON , allow TX decimal 17
    }
    putc('\n'); //this is so upon enter, it avoids things like 222You entered
    *s='\0';
	//TTYINPUTCOUNT=0;//added this so when read the stack it clears it
    return p; //return the address of S stored in P.
}

int randomizer(int min, int max)
{
    int range = max - min;
    return min + rand(rand(rand(rand()))) % range;
}

/*
@return A shared pointer to a random MOTD
*/
void randomMOTD(char *buffer)
{
    const char error[] = "se estás a ler isto, o código falhou\r\n";
    const char *arr[] = {
        "Sabias que o 12-ETC é o melhor?\r\n",
       // "Este programa cabe em 27Kbytes e usa pouca RAM.\r\n",
        "Se o teu terminal suportar, há texto colorido, permitindo criar imagens!\r\n",
        "Já tentaste desligar e voltar a ligar?\r\n",
        "Usa 'clear' para limpar o ecra!\r\n",
      
        "Para mais textos como estes, usa 'motd'.\r\n",
       // "Possivelmente funcional!\r\n",
       // "Putty tem melhor experiencia de uso, mas realTerm tem a capacidade de carregar códigos.\r\n",
       // "Na expocolgaia 2023, Marta não fez uma demonstração prática pois o SBC1802 tinha sido danificado por ESD e partes como o socket da ROM serem SMD e muito pequenos.\r\n",
       // "Usando comandos para controlar o timer/contador do CPU, é possivel calcular a velocidade do CPU mas tambem fazer multi tasking.\r\n",
		"Se backspace não funciona, usa ctrl+H\r\n",
       // "ENQ é uma sequencia enviada para saber qual é o terminal a ser utilizado no momento.\r\n",
       // "Se fosse possivel serializar os bytes de imagens e enviar, o código seria mais rápido.\r\n"
        
    };


    int number = randomizer(0, sizeof(arr) / sizeof(char *));

    const char *output = NULL;
    if (number >= 0 && number < sizeof(arr) / sizeof(char *)) {
        output = arr[number];
    } else {
        output = error;
    }

    strcpy(buffer, output);
}

char *
strtok(s, delim)
char *s;
const char *delim;
{
    char *spanp;
    int c, sc;
    char *tok;
    static char *last;


    if (s == 0x00 && (s = last) == 0x00) {
        return (0x00);
    }

    /*
     * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
     */
cont:
    c = *s++;
    for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
        if (c == sc) {
            goto cont;
        }
    }

    if (c == 0) {		/* no non-delimiter characters */
        last = 0x00;
        return (0x00);
    }
    tok = s - 1;

    /*
     * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
     * Note that delim must have one NUL; we stop if we see that, too.
     */
    for (;;) {
        c = *s++;
        spanp = (char *)delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0) {
                    s = 0x00;
                } else {
                    s[-1] = 0;
                }
                last = s;
                return (tok);
            }
        } while (sc != 0);
    }
    /* NOTREACHED */
}

	char isIIC_STARTED;


/*
typedef struct  {

		void* testeda;

	}funcs const * const libs = 0xF000;
*/

void playSound(char channel, unsigned int frequency, char left, char right, char volume, char waveform, char pulsewidth) {
  gpu_setPortAddress(0, 0x1F9C0 + channel*4, 1);
  write_GPU(0x03, frequency & 0xFF);
  write_GPU(0x03, (frequency >> 8) & 0xFF);
  write_GPU(0x03, ((right & 1) << 7) | ((left & 1) << 6) | (volume & 0x3f));
  write_GPU(0x03, ((waveform & 3) << 6) | (pulsewidth & 0x3f));
}

void main()
{
	//int registerD=0;
	//int seconds=0;
	//int minutes=0;
	//int hours=0;
	
	char* testbank=(char*)0xFFFF;
	char resp;
	char *test;
    char buffer[1024]; //main user input
    //char old_buffer[8][1024]; //we can remember now the previous 8 commands //there goes 8Kbytes
	//char junkbuffer[4096];//small buffer for functions that may need one
	unsigned long int checksumfromload;
	unsigned long int checksumval;
    char motdmessage[256];
    int randomNumber = rand(rand());
    char str[20]; // declaring a character array
    char cin[4];
    //char cin2[4];
	//int SPICOMMAND = 0x00;
	//int SPIBYTE = 0x00;
	//char LOADmem[16384];
	char *progmemory = (char*)0x9000;
    char *token;
    int i = 0;
	//int *malloctest;
    char command[64], var1[64], var2[64], var3[64], var4[64];
    char * flag=(char *)0xfff0; //point to a piece of ram
	int * externalprogramid=(int *)0xffe0; //point to this address, this has the ID of the code to run on the other banks
	char * bankbootindication=(char *)0xffe1; //point to this address, this has the boot/bank return, when its the first boot data is random, but from bank return, its 0x32
//    char *flash_buffer[8192];
	int rgbloop=0;
    char *commandpt = command;
    char *var1pt = var1;
    char *var2pt = var2;
    char *var3pt = var3;
    char *var4pt = var4;
	int outputbuffer = 0;
	char stamps;
	long double expressionresultholder = 0;
	
    gpiocount = 1; //because of SCC2692
//    memorydisplay = 0; //so it shuts up
    *flag=0; //set flag to 0 //
	*externalprogramid=0;//0 is BIOS ID
	
	/*
	for(i=0;i<sizeof(TTYINPUT);i++){
		TTYINPUT[i]=0x00;//clear buffer on reboot
	}
	TTYINPUTCOUNT=0; //we shouldnt have any chars there yet */
	//fault here
	asm("	XID\n");
	asm("	CID\n");
    asm("	req\r\n"); //turn Q off
	asm("	seq\r\n");
   // asm("	ldAD R1,_INTHANDLER  ;.. R1 = INTERRUPT PC\r\n"); //set R1 to point to the interrupt routine
	//asm("	ldAD R1,_INT2  ;.. R1 = INTERRUPT PC\r\n");//set R1(interrupt counter) to execute our subroutine
	out(1,0x03);
    out(2,0x80); //10001001 //so port C are inputs for setting or quiting code
	out(4,0x80); //IO 4 is now having its port A as bidirectional <-> 1100 0000 0xC0
    out(3,0x80);
    out(1,0x00);
	

	//out(1,0x01);
	//out(4,0xFF);//init this for parallel bus
 #if(SCC == 1 || SCC == 2)
    //printf("Starting IO....\r\n");
	


	
	
	//out(ADCIO,0x91);//override to make it accept the ADC
	
	
    asm("	seq\r\n"); //turn Q on
	//asm("	XID\n");
    out(1,0x01);//select port b
	//controlWrite(0x01);
    out(4,0x02); //CRA
    out(6,0x10); // 00010000 set pointer to MR1A

    out(4,0x00); //adress 00 //MR1A//MR2A
	
   // out(6,0x53); //0101 0011 decimal 83 hex 0x53
    out(6,0xD3); //1101 0011 decimal  hex 0xD3
	
    out(6,0x07); //00 0 0 0111 //normal,no rts control,no cts control, 1 stop bit decimal
	
	
	//this has been serialized
	

    out(4,0x02); //CRA
    out(6,0x05); // 0000 0101


	
	
    out(4,0x01); //csra
    out(6,0xBB); //0100 0100 300 bound but in test mode wich is the one we use, its 28.8Kboundrate       //9600 bound rate is 1011 1011 we use 9600 for now 18/11/2023
	//out(6,0x66);    //0110 0110 1200 baud but when in BRG test it gives us 115200baudrate
	//out(6,0xCC); //1100 1100 19200 baud ACR[7]=1 // ACR[7]=0 then 38.4K
	
	//out(4,0x04);
	//out(6,0x80); //set ACR[7]=1, set to 0x00 for ACR[7]=0
	
    out(4,0x05); //R5 :: IMR interrupt mask register
    out(6,0x02);//0000 0010 //for when only Rx of channel A is ready

    out(4,0x02); //CRA
    out(6,0x05); // 0000 0101
	

	
    inp(6); //read register two wich changes to BRGTEST, to make it from 300 bound to 28.8Kboundrate
	
	out(4,0x04);
	out(6,0x80); //set ACR[7]=1, set to 0x00 for ACR[7]=0
	
	
   // delay(1000);
    asm("	req\r\n"); //turn Q on
	
	//printf("canal 1 ligado\r\n");
/* 	out(1,0x01);//select port b
	//controlWrite(0x01);
	out(4,0x0A); //CRB
    out(6,0x10); // 00010000 set pointer to MR1A
	out(4,0x08); //adress 08 //MR1b//MR2b
    out(6,0x53); //010 10 0 11 decimal 83 hex 0x53
    out(6,0x07); //00 0 0 0111 //normal,no rts control,no cts control, 1 stop bit decimal
	
	out(4,0x0A); //CRB
    out(6,0x05); // 0000 0101
	out(4,0x09); //csrb
    out(6,0xBB); //0100 0100 300 bound but in test mode wich is the one we use, its 28.8Kboundrate       //9600 bound rate is 1011 1011 so BB
	out(4,0x0A); //CRB
    out(6,0x05); // 0000 0101
	
	
	
	printf("canal 2 ligado\r\n");
	
	out(4,0x04); //Aux. Control Register (ACR)
	out(6,0x60);//0110 0000
	
	out(4,0x00); //C/T Upper Preset Value (CRUR)
	out(6,0x01);
	out(4,0xFF); //C/T Lower Preset Value (CTLR)
	out(6,0x0A);
	out(4,0x01); //Start Counter Command
	inp(6);
	out(4,0x0D); //Output Port Conf. Register (OPCR)
	out(6,0x04); //C/T OUTPUT */
	
#endif 

    printf("\033[2J");
    //printf("\033[H"); //move cursor to home position, 0,0, so the top of the display
    printf("\x1b[0m");

	isIIC_STARTED=0;
	
	
	
	out(1,0x01);
	out(4,0x0B);//Watchdog and Programmable timer rates
	out(7,0x30);//set RS0-RS2 and no timer base interrupts
	out(4,0x0C);//interrupt enables
	out(7,0x00);//none are allowed
	initScreen();
    set_vga_cursor(0,0);
    set_vga_color(0x01);
    //set_output_mode(OUTPUT_BOTH);

    asm("   SEQ\n");
	PCF_INIT();
    SERIALMODE=0;
	//asm("	XID\n");
    printf("Hi Marta!\r\n");
	if(!TEF4()){//boot switch, if EF4 is grounded, then we autorun serially loaded code, else we go to the BIOS
	
		//out(1,0x20);//switch to NVRAM
		if(progmemorysize>0){
			
			
			if(checksum(progmemory,progmemorysize)==checksumfromload){
				
				printf("\033[37;42mChecksum....OK! 0x%lx\033[0m\r\n",checksumval);
				printf("Running %d bytes!\r\n",progmemorysize);
				//out(1,0x20);
				asm("	lbr 0x9000\r\n");//execute the code in ram
			}else{
				
				printf("\033[37;41mChecksum fault. Expected: 0x%lx. Got: 0x%lx\033[0m\r\n",checksumfromload,checksumval);
				
			}
			
		}else{
			printf("boot exception: no serially loaded code to autorun at boot.\r\n");
		}
	}
    printf("Use 'help' for command list\r\n");
    printf("P.O.S.T. OK V1.4\r\n");
    printf("[39;49m \r\n");
    randomMOTD(motdmessage);
    printf(motdmessage); //random MOTD



        programbreak: //for programs that support pressing x to force them to stop
		asm("postboot:\r\n");//flag used in the returnerfrombanks tool
		asm("	req\r\n");
	    while(1){
			
		//CTS goes low
		out(1,0x01);
		out(4,0x0E);
		out(6,0x01);
		//------------
		
        printf("Ready!\r\n");
		printf(">");
#if(SCC==2)
	   //gets_asm(buffer);
       gets(buffer);
#endif

#if(SCC==0)
	   gets(buffer);
#endif

		//CTS goes high
		out(1,0x01);
		out(4,0x0F);
		out(6,0x01);
		//-------------
		printf("\r\n");

		asm("pastgets:\r\n");//used so after interrupts from this
		// asm("	ldAD R1,_INTHANDLER  ;.. R1 = INTERRUPT PC\r\n"); //set R1 to point to the interrupt routine
        token = strtok(buffer, " ");//changed from buffer to TTYINPUT
        strcpy(command, token);
        memset(var1,0,sizeof(var1)); //clear out this buffers
        memset(var2,0,sizeof(var2));
        memset(var3,0,sizeof(var3));
        memset(var4,0,sizeof(var4));  ////we should optimize this
        for (i = 0; i < 4; i++) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                switch (i) {
                case 0:
                    strcpy(var1, token);
                    break;
                case 1:
                    strcpy(var2, token);
                    break;
                case 2:
                    strcpy(var3, token);
                    break;
                case 3:
                    strcpy(var4, token);
                    break;
                case 4:
                    break;//if too many arguments are sent, kill them
                }
            } else {
                //  printf("Error: Insufficient input\r\n");
                // return 1;
            }
        }//end of it
        if(strcmp(command,"help")==0) {
			//case 'h':
			//printf("%c\r\n",cin);
			printf("\r\n");
			printf("help:\r\n basic:\r\n");
			printf("input <IO chip> <Port>: reads IO ports\r\n");
			printf("output <IO chip> <Port> <Value>: Acess to IO outputs\r\n");
			printf("random <opt1: min> <opt2: max>: randomize the random value\r\n");
			printf("about: credits and such\r\n");
			printf("clear: wipes screen\r\n");
			printf("info: returns information about the SBC1806 state\r\n");
			//printf("motd: mensagem aleatória\r\n");
			//printf("BIOS: acessa as definições da BIOS\r\n");
			printf("fract:  find out :)\r\n");
			printf("I2Cstart <address in dec> <R/#W bit>\r\nI2Cstop\r\nI2Cwrite<byte>\r\nI2Cread <ACK/#NACK>\r\nflashrd <device address> <start address> <bytes to read>\r\nwritemem\r\n");
		//}else if(strcmp(command,"fract")==0) {
			//case 'r':
	
			// printf("\033[2J");
			//printf("\033[H"); //move cursor to home position, 0,0, so the top of the display
			//break;
				
				//mandelbrot();
				
        } else if(strcmp(command,"I2Cstart")==0) {
			//syntax:
			//I2Cstart <address> <0 for write 1 for read>
			
			
			//if((0x00<=atoi(var1)<=0x7F) & (0x00<=atoi(var2)<=0x01)){
			if((0x00<=hexStringToInt(var1)<=0x7F) & (0x00<=atoi(var2)<=0x01)){
			Start();//start i2c
			
			//isIIC_STARTED=addressDevice((char)atoi(var1),(char)atoi(var2));
			isIIC_STARTED=addressDevice((char)hexStringToInt(var1),(char)atoi(var2));
			
			if(!isIIC_STARTED){
				printf("Unable to address device.\r\n");
				Stop();//stop i2c
			}else{
				
				printf("Started communications with device:%cx\r\n",(char)atoi(var1));
				
			}
			
			}else{
				
				printf("Invalid arguments\r\n");
				
			}
			
		} else if(strcmp(command,"I2Cwrite")==0) {
			
			if(isIIC_STARTED){
				resp=sendByte((char)hexStringToInt(var1));
				if(resp){
					printf("Received ACK\r\n");
				}else{
					printf("Received NACK\r\n");
				}
			}else{
				printf("I2C BUS not ready.\r\n");
			}
			
        } else if(strcmp(command,"I2Cread")==0) {
			if(isIIC_STARTED){
				printf("Read back:%cx\r\n",readByte((char)atoi(var1)));
			}else{
				printf("I2C BUS not ready.\r\n");
			}
			
        } else if(strcmp(command,"I2Cstop")==0) {
			
			if(isIIC_STARTED){
				
				Stop();//stop i2c
				printf("Ended I2C communication\r\n");
				isIIC_STARTED=0;
			}else{
				printf("There is no valid I2C communication occurring\r\n");
			}
			
        } else if(strcmp(command,"flashrd")==0) {
			
			//I2C_flash_read(char device,unsigned int address,unsigned int array_size,char* array)
			if(atoi(var3)<=8192){
			I2C_flash_read((char)atoi(var1),atoi(var2),atoi(var3));
			}else{
				printf("Requested size too big, max is 8192\r\n");
				printf("flashrd <device address> <start address> <bytes to read>\r\n");
			}
			
        } else if(strcmp(command,"test")==0) {
            //test banks
			playSound(0, 1181, 1, 1, 255,0, 50);

		} else if(strcmp(command,"time")==0) {

            out(1,0x01);
		  
            out(4,0x0E);
            out(7,0x06);
            rtc_check();

        } else if(strcmp(command,"settime")==0) {
            printf("format: hh:mm:ss\r\n");
            RTC_settime(atoi(var1),atoi(var2),atoi(var3));
			
		} else if(strcmp(command,"load")==0) {
				
				
					//printf("75ms between chars required.(100ms for better results)\r\n");
					//getsSized(progmemory,atoi(var1));
					

                    #if(SCC==2)
                       
					//CTS goes low
					out(1,0x01);
					
					out(4,0x0E);
					out(6,0x01);
					//------------
	                // getsSized_asm(progmemory,atoui(var1));
					progmemorysize=getsRTS_asm(progmemory);
					 //CTS goes high
					out(1,0x01);

					out(4,0x0F);
					out(6,0x01);
					//-------------
                    #endif

                    #if(SCC==0)
                     printf("A 100ms char delay is needed.\r\n");
	                 getsSized(progmemory,atoi(var1));
                    #endif
					//test =  (char*)malloc(atoi(var1));
					//if(test){
					//getsSized(test,atoi(var1));
					printf("Code has been loaded.\r\n");
					printf("Recebidos: %d bytes\r\n",progmemorysize);
					checksumfromload=checksum(progmemory,progmemorysize);
					printf("Checksum: 0x%lx\r\n",checksumfromload);
				//	progmemorysize=atoi(var1);
					
				
	
				
				
			} else if(strcmp(command,"read")==0) {
				if(atoi(var1)<=0){
					printf("Error: you must read one or more bytes (1-8192)\r\n");
				}else{
					
					for(i=0;i<atoi(var1);i++){
						printf("%cx,",progmemory[i]);
					}
					printf("Pronto\r\n");
					//printf("tamanho: %d bytes\r\n",progmemorysize);
					//printf("Checksum: %lx\r\n",checksum(progmemory,progmemorysize));
					
				}
				
			} else if(strcmp(command,"delay")==0){
				
				if(atoi(var1)>0){
				delay(atoi(var1));
				}else{
					printf("Invalid argument: VAR1 >0\r\n");
				}
				
			} else if(strcmp(command,"run")==0) {
				//if(progmemorysize<=0){
				//	printf("Error: code size is 0 Bytes, you need to load the code first by using: 'LOAD <size>'\r\n");
				//}else{
					
				printf("running %d bytes....\r\n",progmemorysize);
				
				asm("	lbr 0x9000\n");//execute the code in ram
				//}
				
			} else if(strcmp(command,"erase")==0) {
				
				printf("Erasing %d Bytes\r\n",progmemorysize);
				for(i=0;i<progmemorysize;i++){
					progmemory[i]=0xFF;//hard wipe
				}
				//free((unsigned char *)test);
				printf("Done\r\n");
				progmemorysize=0;
				
	
		} else if(strcmp(command,"info")==0) {
	
			infodisplay();
	
	
	
		}else if(strcmp(command,"motd")==0) {
	
			randomMOTD(motdmessage);
			printf(motdmessage); //random MOTD
		}else if(strcmp(command,"about")==0) {
			
	
			/*printf("\033[48;2;252;0;223m            \033[m\r\r\n");
	
			printf("\033[48;2;0;252;240m            \033[m\r\r\n");
	
			printf("\033[48;2;255;255;255m            \033[m\r\r\n");
			//printf("");
			printf("\033[48;2;0;252;240m            \033[m\r\r\n");
			//printf("");
			printf("\033[48;2;252;0;223m            \033[m\r\r\n");
			//printf("");
			printf("\033[0m");*/
			///////////
			printf("Made by: MsAlPatina!\r\r\n");
			printf("Major thanks to: Bill, Grisses, nicky_5 and Zled!\r\r\n");
			printf("https://github.com/MsAlPatina\r\r\nhttps://github.com/Grissess\r\r\nhttps://github.com/bill2009\r\r\nhttps://github.com/zledtriac\r\r\nhttps://github.com/nicky-5\r\r\nhttps://github.com/MsAlPatina/SBC1806\r\r\n");
			
			
	
			//printf("\033[48;5;194m");
			//printf("  \r\r\n");
			printf("\033[0m");
		}
	
	
	
		else if(strcmp(command,"rgb")==0) {

            
	
			if(atoi(var1) <= 0) {
				printf("Erro: arg1 deve ser maior que 0 (define o numero de ciclos).\r\r\n");
	
			} else {
				if(atoi(var2)<=0) {
					printf("Aviso: arg2 nulo defenido valor para 250ms.\r\r\n");
					strcpy(var2,"250");
				} else {
					printf("Party time!\r\r\n");
				}
/*
				printf("\033[41m            \033[m\r\r\n");
				//printf("            \r\r\n");
				printf("\033[43m            \033[m\r\r\n");
				//printf("            \r\r\n");
				printf("\033[42m            \033[m\r\r\n");
				//printf("            \r\r\n");
				printf("\033[44m            \033[m\r\r\n");
				//printf("            \r\r\n");
				printf("\033[45m            \033[m\r\r\n");
				//printf("            \r\r\n");
	
				printf("\033[32;49m");
				printf("Embrace the rainbow!! \r\r\n");*/
				printf("Press c to stop\r\r\n");
	
				printf("\033[0m");
				
				//CTS goes low
				
				//------------
	
				//printf("%c",0x13); //so no sending stuff
				//while(getINint()){
					//out(1,0x01); //port B
				for(rgbloop=0; rgbloop<=atoi(var1)||!(inp(6) & 0x01); rgbloop++) {
					//printf("%c\r\r\n",cin);
						//out(1,0x01); //port B
					randomNumber = randomizer(1, 7); //cap it to 7
						randomNumber |= randomizer(16,240);
					//printf("random number = %d\r\r\n",randomNumber);
					out(1,0x00);
					
					out(4,randomNumber);
					delay(atoi(var2));
						
						
						
						//if(inp(6) & 0x01){
						
						//out(1,0x01); //port B
						
						//out(4,0x03); //select RXHA
						//buffer = inp(6);
							if(i2c_readdata(keyboard) == 'c'){
							    asm("	lbr rgbbreaker\r\n");
							}
                        
						//}
				}
					asm("rgbbreaker\r\n");
					//CTS goes high
					
					//-------------
				//}
					
				//printf("%c",0x11); //now you can send stuff
			}
		} else if(strcmp(command,"input")==0) {
			inputprompt(var1,var2);
		} else if(strcmp(command,"output")==0) {
					var3pt = var3;
					outputbuffer = (int)expression(var3pt,&var3pt);
			if(outputbuffer < 0 || outputbuffer > 255) {
				printf("Value to output goes from 0 to 255\r\n");
			}else if(strcmp(var3,"ans")==0){
					outputprompt(var1,var2,(int)ANS);
				} else {
					//printf()
					//var3pt = var3;
				//outputprompt(var1,var2,outputbuffer);
				outputprompt(var1,var2,hexStringToInt(var3));
			}
				
				
		
			//sleepMODE(atoi(var1),atoi(var2));
		} else if(strcmp(command,"random")==0) {
			//case 'r':
			//printf("%c\r\n",cin);
	
			if(atoi(var2) > atoi(var1)) {
				printf("%d\r\n",randomizer(atoi(var1), atoi(var2)));
			}
	
			printf("Rand used to be: %d\r\n",randomNumber);
			randomNumber = rand(rand());
			printf("Now rand is: %d\r\n",randomNumber);
			//break;
		} else if(strcmp(command,"clear")==0) {
			//case 'r':
	
			printf("\033[2J");
			//printf("\033[H"); //move cursor to home position, 0,0, so the top of the display
			//break;
		}else if(strcmp(command,"shutdown")==0) {
			//case 's':
			printf("%c\r\n",cin);
			printf("\a"); //plays a sound on the terminal
			printf("Shutting down\r\n");
			//delay(2000);
			printf("\a"); //plays a sound on the terminal
			// out(1,0x00); //shut down all pins
			printf("Bye!\r\n");
			//asm("idl\r\n"); //we halt the CPU
			//exit(0); prints exit, then the value given, then goes in a while loop
			while(1) {
				asm("	req\r\n");
				delay(250);
				asm("	seq\r\n");
				delay(250);
			}
				
				
			}else if(strcmp(command,"reboot")==0){
				
				asm("	IDL\r\n");//idling will cause S0 to stop, triggering the watchdog timout and reboot the system
				
		
		} else {
	
			//calculator(command,var2,var1);
            printf("command not recognised.\r\n");
			commandpt = command;
            /*
			ANS = expression(commandpt,&commandpt);
				
				
				if(ANS==0){
					printf("%f\r\n",ANS);
				}else{
				
					if(ANS < 1000 & ANS >= 0.001) {
						printf("%f\r\n",ANS); //in this case, we pass the expression, it must be 2-34-54, not 2 - 34 - 54
					}
					if(ANS > 1000) {
						floatToScientific(ANS);
						printf("\r\n");
					}
					if(ANS < 0.001) {
						floatToScientific(ANS);
						printf("\r\n");
			
					}
				}*/
			//
			//printf("\a"); //plays a sound on the terminal
	
			
	
		}


	}
}


void syscall() { //where the global functions can be called from
    asm(" SET SAVELOC,$\n");
	asm("	ORG 0x7F00\n");
	asm("	dw _printf\n"); //store the address of our functions.
    asm("   dw _mputc\n");
    asm("   dw _mgetc\n");
    asm("   dw _getKeyboard\n");
    asm("   dw _sccOUT\n");
    asm("   dw _sccIN\n");
    asm(" org SAVELOC\n");
    /*  
      asm(" SET SAVELOC,$\n");
    asm("    org 0x7FFC\n");  //todo:: add a requester tool, so we can go here and request either to return to the bios or get something from here like the printf
    asm("    lbr postboot\n");//we have an 
    asm(" org SAVELOC\n");
    */

}

#if(SCC==0)
char*   getsSized(char *s,unsigned int size)//changes for speed
{
	
	unsigned int received=0;
	//int memoryreceivedbytes = 0;
    char ch,*p;
	progmemorysize=0;
    //usermem = 0;
    p=s;
//	
	asm("	seq\r\n");
    while (received < size) { //replaced \r\n with 
        //printf("%c",0x13); //send DC3, so XOFF, stop TX decimal 19
			ch = getc();
            *s++ = ch;
            usermem++; //increment the memory pointer
			received++;
			
			progmemorysize++;
			//printf("%l\r\n",sized);
			//printf("got:%x\r\n",ch);
			//printf("user memory:%d\r\n",usermem);
			//printf("got:%d\r\n",memoryreceivedbytes);
        }
       // printf("%c",ch);
        //printf("%c",0x11); //send DC1, so XON , allow TX decimal 17
    
   // printf("\r\n"); //this is so upon enter, it avoids things like 222You entered
    *s='\0';
	asm("	req\r\n");
    return p; //return the address of S stored in P.
}


char*   gets(char *s)
{

    char ch,*p;
    usermem = 0;
    p=s;
	
    while ((ch=getc()) != '\r') { //replaced \r \n with 
	
        putc(0x13); //send DC3, so XOFF, stop TX decimal 19
        if(ch == 0x08) { //\b is used but i go with 0x08
            *s = '\0';
		  // *s--;
            if(s > p) {
                s--;    //added backspace compatability , so no more pressing enter to undo your error
                putc('\b');
                putc(' ');
            }
        } else {
            *s++ = ch;
            usermem++; //increment the memory pointer
        }
        printf("%c",ch);
        putc(0x11); //send DC1, so XON , allow TX decimal 17
    }
    printf("\n"); //this is so upon enter, it avoids things like 222You entered
    *s='\0';
	//TTYINPUTCOUNT=0;//added this so when read the stack it clears it
    return p; //return the address of S stored in P.
}

#endif








void outputprompt(char* cin,char* cin2,int buffer)
{
    if(strcmp(cin2,"a")==0) {
        out(1,0x00);
		
    } else if(strcmp(cin2,"b")==0) {

        //case 'b':
       out(1,0x01);
	   
    } else if(strcmp(cin2,"c")==0) {

        //case 'c':
        out(1,0x02);
		
    } else if(strcmp(cin2,"ctrl")==0) {
        out(1,0x03);
		
    } else {
        //default:
        printf("only a,b,c or ctrl\r\n");
        //break;
    }

    if(atoi(cin)>=1 ||atoi(cin)<=7) {
        out((char)atoi(cin),buffer);
        //break;
      //break;
    } else {
        //default:
        printf("invalid io it goes from 1-7 \r\n");
        //break;
    }


    printf("Done\r\n");


}


void inputprompt(char* cin,char* cin2)
{

   
    if(strcmp(cin2,"a")==0) {
        out(1,0x00);
		
    } else if(strcmp(cin2,"b")==0) {

        //case 'b':
        out(1,0x01);

		
    } else if(strcmp(cin2,"c")==0) {

        //case 'c':
        out(1,0x02);
    } else if(strcmp(cin2,"ctrl")==0) {

        out(1,0x03);
		
		
    } else {
        //default:
        printf("only a,b,c or ctrl \r\n");
        //break;
    }

    if(atoi(cin)>=2 || atoi(cin) <=7) {
        //printf("%c\r\n",cin);
        printf("Readed: %x \r\n",inp((char)atoi(cin)));
        //break;
    
        //break;
    } else {

        //default:
        //printf("%c\r\n",cin);
        printf("IO Not Recognized, it goes from 2-7\r\n");
        //break;


        //internal i

        //for i
        //break;
    }

}


//void draw_info_box(){
	/*char box[]={
		"┌────────────────────────────────┐",
		"│                                │",
		"├────────────────────────────────┤",
		"│                                │",
		"│                                │",
		"│                                │",
		"│                                │",
		"│                                │",
		"│                                │",
		"│                                │",
		"│                                │",
		"│                                │",
		"└────────────────────────────────┘"
	};*/
	
	//char box[]={"+-------------------+\r\n| info              |\r\n+------+------+-----+\r\n|object|value |units|\r\n+------+------+-----+\r\n| CPU  | 5.00 | Mhz |\r\n+------+------+-----+\r\n| SCC  |3.6864| Mhz |\r\n+------+------+-----+\r\n| GPIO |      | N/A |\r\n+------+------+-----+\r\n| I2C  | 5.00 | Mhz |\r\n+------+------+-----+\r\n| RTC  |32.768| Khz |\r\n+------+------+-----+\r\n"};
	
	
//}
#if(SCC==1||SCC==2)
	#define DUART 1
#endif
#if(SCC==0)
	#define DUART 0
#endif



void infodisplay()
{
    //char * flag=(char *)0xfff0;
   // char buffer[12];
   // printf("SBC1806 INFO\r\n");
    //memdisplay();
   // printf("ROM: 128Kbytes Versão:8.4 18/08/2024\r\n");
   // printf("RAM:128Kb/256Kb\r\n");

   // printf("\033[48;2;37;155;36m User memory.\033[m\r\n");
   // printf("Used: %dB/26kb\r\n",usermem);
	
   // printf("IO:\r\n");
   char table[]={
				 "┌─────────────────────┐\r\n"
                 "│info                 │\r\n"  
				 "├────┬────┬──────┬────┤\r\n"
				 "│Type│Num │ Clk  │Unit│\r\n"
				 "├────┼────┼──────┼────┤\r\n"
				 "│GPIO│01/3│ N/A  │ N/A│\r\n"
				 "├────┼────┼──────┼────┤\r\n"
				 "│SCC │01/1│3.6864│ Mhz│\r\n"
				 "├────┼────┼──────┼────┤\r\n"
				 "│PCF │01/1│5.0000│ Mhz│\r\n"
				 "├────┼────┼──────┼────┤\r\n"
				 "│RTC │01/1│32.768│ Khz│\r\n"
				 "└────┴────┴──────┴────┘\r\n"
	};
	
   
	printf(table);
	//printf(table2,1.000,1.000,999,999,999);
	printf("\r\n\r\n");
	printf("\033[48;2;0;188;212m Program memory.\033[m\r\n");
	printf("Used %dB/16Kb\r\n",progmemorysize);//we use floats for mem since its x.000
   
   // printf("	%d/4 GPIO 82C55\r\n",gpiocount);
  //  printf("	1/1 DUART SCC2692\r\n");
  //  printf("CPU:");
  //  printf("	CLOCK:5Mhz\r\n");
  //  printf("	Tipo:1806\r\n");
  //  printf("SCC2692\r\n");
   // printf("	CLOCK:3.6864Mhz\r\n");
   // printf("	Baund rate: 9600\r\n");
  //  printf("	Max character speed: 960(CPS)\r\n");
  //  printf("\r\n");

}

void I2C_flash_read(char device,unsigned int address,int array_size){
	
	int i;
	char resp;
	char readed;


	if(!isIIC_STARTED){

		Start();//start the I2C BUS
		resp=addressDevice(0x50,1);

		if(resp){

			sendByte((address >> 8) & 0xFF);
			sendByte(address & 0xFF);


		}else{

			Stop();
			printf("I2C BUS EXCEPTION\r\n");
			return;

		}
	}else{
		
		printf("I2C BUS BUSY\r\n");
		return;
	}

	out(1,0x01); //A0 goes high
	out(5,0x45);//re-send start, become master receiver


	//if(!isIIC_STARTED){
		//Start();//start the I2C BUS
		resp=addressDevice(0x50,1);
		
		if(resp){
			(void)readByte(1);//dummy read
			for(i=0;i<array_size;i++){

				//array[i]=readByte(1);
				readed=readByte(1);
				printf("0x%cx,",readed);
			}
			(void)readByte(0);//nack
			printf("Done!\r\n");
			(void)i2c_stopReceiver();
			//Stop();
		
		}else{
			
			Stop();
			printf("I2C BUS EXCEPTION\r\n");
			
		}
		
	//}else{
		
	//	printf("I2C BUS BUSY\r\n");
		
	//}

}

/*
void floatToScientific(float num)
{
    int exponent = 0;

    while (num >= 10.0 || num <= -10.0) {
        num /= 10.0;
        exponent++;
    }

    while (num < 1.0 && num > -1.0) {
        num *= 10.0;
        exponent--;
    }

    printf("%f x 10^", num);
    printf("%d", exponent);
}

*/



//ALPHA
long double expression(char* string, char** lastPosition);

long double atofinternal(char *s, char** lastPosition)
#define isdigit(c) (c >= '0' && c <= '9')
{
    // This function stolen from either Rolf Neugebauer or Andrew Tolmach.
    // Probably Rolf.
    long double a = 0.0;
    int e = 0;
    int c;
    while ((c = *s++) != '\0' && isdigit(c))
    {
        a = a*10.0 + (c - '0');
    }
    if (c == '.')
    {
        while ((c = *s++) != '\0' && isdigit(c)) {
            a = a*10.0 + (c - '0');
            e = e-1;
        }
    }
    if (c == 'e' || c == 'E')
    {
        int sign = 1;
        int i = 0;
        c = *s++;
        if (c == '+') {
            c = *s++;
        } else if (c == '-') {
            c = *s++;
            sign = -1;
        }
        while (isdigit(c)) {
            i = i*10 + (c - '0');
            c = *s++;
        }
        e += i*sign;
    }
    while (e > 0)
    {
        a *= 10.0;
        e--;
    }
    while (e < 0)
    {
        a *= 0.1;
        e++;
    }

    *lastPosition = s - 1;
    return a;
}


long double factor(char* string, char** lastPosition)
{
    long double result;
    if(*string == '(') {
        string++;
        while(*string == ' ') {
            string++;    //skip spaces;
        }
        while(*string != ')' && *string != 0) {
            result = expression(string, &string);
        }
        string++;  // eat the )
    } else {
        result = atofinternal(string, &string);
    }

    *lastPosition = string;

    return result;
}


long double term(char* string, char** lastPosition)
{
    long double numResult = 0;
    long double result = 0;

    while(*string == ' ') {
        string++;    //skip spaces;
    }

    result = factor(string, &string);

    while(*string != 0) {


        while(*string == ' ') {
            string++;    //skip spaces;
        }

        if(*string == '*') { //if we see a * then do the multiplication.
            string++;
            while(*string == ' ') {
                string++;    //skip spaces;
            }
            result *= factor(string, &string);
        } else if(*string == '/') { //
            string++;
            while(*string == ' ') {
                string++;    //skip spaces;
				
            }

            result /= factor(string, &string);
        } else {
            break;
        }

    }
    *lastPosition = string;
    return result;
}


long double expression(char* string, char** lastPosition)
{
    long double numResult = 0;
    long double result = 0;

    while(*string == ' ') {
        string++;    //skip spaces;
    }

    result = term(string, &string);

    while(*string != 0) {


        while(*string == ' ') {
            string++;    //skip spaces;
        }

        if(*string == '+') { //if we see a * then do the multiplication.
            string++;
            while(*string == ' ') {
                string++;    //skip spaces;
            }
            result += term(string, &string);
        } else if(*string == '-') { //
            string++;
            while(*string == ' ') {
                string++;    //skip spaces;
            }
            result -= term(string, &string);
        } else {
            break;
        }

    }
    *lastPosition = string;
    return result;
}







unsigned long int checksum(char *data,unsigned int size){
	unsigned int i;
	unsigned  long int checksum=0x00;
	
	for(i=0;i<size;i++){
		
		checksum+=data[i];
		
	}
	
	return checksum;
	
}



//char buffer[32];




/*
void mandelbrot(){//https://youtu.be/heSGogzIEUM?t=241

	
	int k = 0;
    float i,j,r,x,y=-16;
    char chr[] = " .:-;!/>)|&IH%*#";
	char colors[] = {41,42,43,44,45,46,47,48,49};

    puts("\r\n");
    while(y < 15) {
        y++;
        x = 0;
        while(x < 84) {
            x++;

            i = 0;
            r = 0;
            k = 0;

            while(1) {
                j=r*r-i*i-2+x/25;
                i=2*r*i+y/10;

                if((j*j+i*i) < 11) {
                    if(!(k < 111)) {
                        k++;
                        break;
                    }
                    k++;
                }
                else {
                    break;
                }
                r = j;
            }

            //putchar(chr[k & 15]);
			  printf("\033[1;%dm ",colors[k&15]);
        }
        puts("\r\n");
		
    }

}

*/
	




void rtc_check(){
	
	char flags;
    char seconds;
    char minutes;
    char hours; 
	char zeror;

	out(1,0x01);
	out(4,0x0D);
	flags=inp(7);
	if(flags&0x01){
		printf("\033[37;42mRTC battery OK\033[0m\r\n");
		
	}else{
		printf("\033[37;41;1mRTC battery BAD\033[0m\r\n");
	}
	
	if(flags&0x02){
		
		printf("\033[34;43mPower bus fault\033[0m\r\n");
	}
	
	printf("%x\r\n",flags);

    out(4,0x00);
    seconds=inp(7);
    out(4,0x02);
    minutes=inp(7);
    out(4,0x04);
    hours=inp(7);
	out(4,0x0F);
	zeror=inp(7);
	printf("%cx\r\n",zeror);
	if(zeror){
		printf("Address fault.\r\n");
	}

    printf("%cx:%cx:%cx\r\n",hours,minutes,seconds);
    

}

int hexCharToInt(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';  // '0' -> 0, '1' -> 1, ..., '9' -> 9
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;  // 'A' -> 10, 'B' -> 11, ..., 'F' -> 15
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;  // For lowercase letters 'a' -> 10, ..., 'f' -> 15
    } else {
        return -1;  // Invalid character for hex
    }
}

int hexStringToInt( char *hexStr) {
    int result = 0;
    int i;
    for (i = 0; hexStr[i] != '\0'; i++) {
        int value = hexCharToInt(hexStr[i]);
        if (value == -1) {
           // printf("Invalid hex digit: %c\n", hexStr[i]);
            return -1;  // Return -1 for invalid characters
        }
        result = result * 16 + value;  // Shift the result by 16 and add the new value
    }
    return result;
}

int decimalToBCD(int decimal) {
    int bcd = 0;  // Variable to store the BCD representation.
    int multiplier = 1;  // Multiplier to extract each digit of the decimal number.
 
    while (decimal > 0) {
        int digit = decimal % 10;  // Extracting the rightmost digit of the decimal number.
        bcd += digit * multiplier;  // Adding the digit to the BCD representation.
        multiplier *= 16;  // Multiplying the multiplier by 16 to shift to the next BCD digit.
        decimal /= 10;  // Removing the rightmost digit from the decimal number.
    }
 
    return bcd;  // Returning the BCD representation.
}

void RTC_settime(int hour,int minute,int second){
	
	
	out(1,0x01);
	out(4,0x0E);
	out(7,0x0C);
	
	out(4,0x04);
	out(7,decimalToBCD(hour));
	
	out(4,0x02);
	out(7,decimalToBCD(minute));
	
	out(4,0x00);
	out(7,decimalToBCD(second));
	
	out(4,0x0E);
	out(7,0x04);
	
}
/*

    +-------+-----------+
    |address|type       |
    +-------+-----------+
    |0x00   |seconds    | <----> +1 for alarm reg
    |0x02   |minutes    | <----> +1 for alarm reg
    |0x04   |hours      | <----> +1 for alarm reg
    |0x06   |day        | <----> +1 for alarm reg
    |0x08   |day-of-week|
    |0x09   |months     |
    |0x0A   |year       |
    +-------+-----------+

*/

/*
void sleepMODE(char timed,char mode){
	//,char preset removed
	//IDLES THE CPU
	while(1){
		asm(	        "	CIE ;enable interrupts from counter for now, enable outside\r\n"
						"	XIE ;enable external interrupts\r\n"
						"	REQ ;turn off Q\r\n");
		if(timed){
			//we set timer
			
			//asm("");
			
			switch(mode){
				
				case 0:
					asm("	STM\r\n"); //set timer mode and start
					break;
				case 1:
					asm("	SCM1\r\n");//set counter mode 1 and start
					break;
				case 2:
					asm("	SCM2\r\n");//set counter mode 2 and start
					break;
				case 3:
					asm("	SPM1\r\n");//set pulse width mode 1 and start
					break;
				case 4:
					asm("	SPM2\r\n");//set pulse width mode 2 and start
					break;
				case 5:
					break;
				
			}
			
			
		}
			
		asm("	ldAD R1,_INTHANDLER  ;.. R1 = INTERRUPT PC\n");//get the interrupt vector ready
		asm("	IDL\n");//pause cpu
		//-----//
		//-----//
		asm("	nop\n");
		asm("	nop\n");
		asm("	lbr _exitsleep\n");
		
		//asm("	B4 _exitsleep\r\n");//if B4 gets pulled low then we exit sleep mode else we loop, this is because of the watchdog of the RTC	
		
		
	}
	asm("_exitsleep:\n");
	asm("	SEQ ;turn on Q\n");
	printf("<info>::Left sleep mode (interrupt was called)\n");
    delay(1000);
    asm("   REQ\n");
}

*/



/*
void INT_HANDLER(){
	asm("_INTHANDLER: \n");
	asm("	SAV\n");
    asm("   RSXD R15\n");
    asm("   RLDI R0,_S130812123\n");
	asm("   RLXA R15\n");
	asm("	RET\n");
	asm("	LBR _INTHANDLER\n");
	//putc('>');
	
	asm("_S130812123:\r\n");
    asm("   db \"hello world!\",00 \r\n");
	
	
	
}*/


#include <olduino.c>
#include <stdlib.c>

#if(SCC == 0)
void includeser2()
{
    asm("BAUDRATE EQU 	9600\r\n");
    asm(" include xrwjrT3.inc\r\n");
    asm(" include serwjrT3.inc\r\n");
}
#endif


