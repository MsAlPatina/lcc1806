void Start(void){
	
	out(1,0x01);
	out(5,0xC5);
	
}

char addressDevice(unsigned char address, unsigned char RW){
	//RW=0 is write, RW=1 is read
	address<<=1;//shift left LSB is now 0
	address|=(RW&0x01);
	out(1,0x01);
	out(1,0x00);
	out(5,address);
	out(1,0x01);
	
	while(inp(5)&0x80){
        asm("    NOP\n");
    }
	
	if((inp(5)&0x08)){
		printf("NACK!\r\n");
		return 0;//NACK
		//printf("NACK!\r\n");
		
	}else{
		printf("ACK!\r\n");
		return 1;//ack
	}
	
	
	
}

	
char sendByte(unsigned char byte) {
    out(1,0x00);
    out(5,byte);
    
    out(1,0x01);
    while(inp(5)&0x80){
        asm("    NOP\n");
    }

    if((inp(5)&0x08)) {
        return 0;//NACK        
    }else{
        return 1;//ack
    }
}

unsigned char readByte(unsigned char ack) {
    unsigned char readedByte;

    if(!ack) {
      out(1, 0x01);
      out(5,0x40);  //if ack == 0; then send nack
    }

    out(1, 0x00);
    readedByte = inp(5);

    out(1,0x01);
    while(inp(5)&0x80){
        asm("    NOP\n");
    }

    return readedByte;   
}

unsigned char i2c_stopReceiver(void) {
    out(1,0x01);    //STOP
    out(5,0xC3);

    out(1, 0x00);
    return inp(5);
}

void i2c_stopTransmitter(void) {
    out(1,0x01);    //STOP
    out(5,0xC3);
}

void Stop(void) {
    out(1,0x01);    //STOP
    out(5,0xC3);
}
