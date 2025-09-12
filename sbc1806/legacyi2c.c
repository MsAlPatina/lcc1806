//I2C lib

void initializei2c(){

    out(1,0x01);
    out(5,0x80);
    out(1,0x00);
    out(5,0x55);
    out(1,0x01);
    out(5,0xA0);
    out(1,0x00);
    out(5,0x1C);
    out(1,0x01);
    out(5,0xC1);
}

char startI2C(char address, char rw){
	
    
    address<<=1;//shift left LSB is now 0
	address|=(rw&0x01);

    out(1,0x01);
    //BB=1 -> free BB=0 -> busy //should be PIN
    //check if there is any data transfer
	/*while(!(inp(5)&0x80)){
        asm("    NOP\n");
        //printf("Waiting while addressing...\r\n");
        //print("Waiting on PIN1\r\n");
        out(1,0x01);
    }*/

	
	out(1,0x00);
	out(5,address);
	out(1,0x01);
	out(5,0xC5);

    while((inp(5)&0x80)){

        print("Waiting on PIN2\r\n");
        out(1,0x01);
    }



    out(1,0x01);
    //print("Device addressed, now its fate:");
    if((inp(5)&0x08)){
        print("NACK\r\n");
        return 1;
    }else{
        if(rw){
        // **** IMPORTANT: Perform a single dummy read here to start the process ****
        out(1, 0x00);
        (void)inp(5);
        }

        print("ACK\r\n");
        return 0;
        
    }

    

	
}

char sendByte(char data){


    out(1,0x01);
    while((inp(5)&0x80)){
        print("Waiting on PIN3\r\n");
        out(1,0x01);
    }
    out(1,0x00);
    out(5,data);

    out(1,0x01);
    while(!(inp(5)&0x80)){
        print("Waiting on PIN4\r\n");
        out(1,0x01);

    }

    return (inp(5)&0x08);

}



char readbyte(char ack) {
    char data;

    // STEP 1: Wait for the current byte reception to complete.
    // The PIN bit (S1, bit 7) will be 0 when a byte is ready in the S0 register.
    out(1, 0x01); // Select control register S1
    while (inp(5) & 0x80) {
        // Waiting for PIN to become 0...
    }

    // STEP 2: Before reading the data, decide if you will ACK or NACK it.
    // This configures what happens *after* the upcoming read from S0.
    if (!ack) {
        // To send a NACK for this byte (signaling the end of transmission),
        // we must set the ACK bit (ES1) to 1.
        out(5, 0x44); // Master Receive Mode + NACK for this byte
    }
    // If 'ack' is true, we don't need to do anything. The controller is already
    // in master-receive mode (from the start() call) and will ACK by default.

    // STEP 3: Read the data from the S0 register.
    // This action reads the byte and *also* sends the pre-configured ACK/NACK.
    out(1, 0x00); // Select data register S0
    data = inp(5);

    return data;
}
/**
 * @brief Generates a stop condition on the I2C bus.
 */
void stopI2C() {
    out(1, 0x01); // Select control register S1
    out(5, 0xC3); // Set STO=1 to generate a STOP condition.
                  // This also clears the master mode.
    print("stopped\r\n");
}



