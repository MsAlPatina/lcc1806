;GPU.inc
;a simple library for interfacing the VERA-X card

;prequesits:
;databus of the card be on port A of IO 2
;address of the card on port B of IO 2
;WR on pin 7 of port C of IO 2 0x80
;RD on pin 6 of port C of IO 2 0x40
;CS tied low
;IO 2 set on mode 0x80

_write_GPU:
;char address,char data

    ; out(1,0x01);//PORT B (ADDRESS of GPU card)
    ; out(2,address);
    ; out(1,0x00);//PORT A
    ; out(2,data);
    ; out(1,0x02);
    ; out(2,RD); //lower WR
    ; out(2,RD|WR); //lower WR

	;latest
    ;out(1,0x01); //port B
    ;out(4,address);
    ;out(3,data);

    ;go to port B
    SEX R2
	DEC R2
	LDI 0x01
	STR R2
	out 1
    
    ;------------

    
	DEC R2
	GLO R12
	STR R2
	out 4 ;make me match GPU_IO define
    

    ;go to port A
    
	DEC R2
	LDI 0x00
	STR R2
	out 1
    
    ;------------

    
	DEC R2
	GLO R13
	STR R2
	out 3 ;IO where the GPU is mounted at

    Cretn


_read_GPU:

    ;go to port B
    SEX R2
	DEC R2
	LDI 0x01
	STR R2
	out 1
    
    ;------------

    
	DEC R2
	GLO R12
	STR R2
	out 4 ;make me match GPU_IO define
    

    ;go to port A
    
	DEC R2
	LDI 0x00
	STR R2
	out 1
    
    ;------------

    
	
	
	
	inp 3; read the GPU
	plo R15 ;return value


	Cretn


;void gpu_clear_display(){
;    int i;
;    int map_dim_size[] = {32, 64, 128, 256};
;    int map_size = map_dim_size[MAP_HEIGHT] * map_dim_size[MAP_WIDTH];//1,2
;    gpu_setPortAddress(0, SCREEN_MAP_ADDR, 1);
;    for(i=0;i<map_size;i++){
;       write_GPU(0x03, 0x00);   
;       write_GPU(0x03, 0x00);  
;   }
;}

_gpu_clearDisplay:
	;fast screen clean
	RLDI R15,0x2000
$$loop_clearDisplay:
	ldi 0x03 ;reg 3
	plo R12
	ldi 0x00 ;write 0
	plo R13
	Ccall _write_GPU;write twice
	Ccall _write_GPU
    dbnz R15,$$loop_clearDisplay ;loop
	Cretn





;_loadFont:
;copies an array that contains the font to memory

;arg1 is the array
;arg2 is the size


