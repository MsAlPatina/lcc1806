//PCF8584_DRIVER.h

void PCF_INIT();
void I2C_SEND(char X);
char I2C_READ();
void I2C_endTransmission();
char I2C_beginTransmission(char address);
void I2C_ACK();
void I2C_NACK();
char I2C_request(char address);
void includerofPCF(){
	
	asm("	include PCF8584_DRIVER.inc\n");
}

