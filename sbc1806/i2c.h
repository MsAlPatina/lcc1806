
void initializei2c(void);
char startI2C(char address, char rw);
void stopI2C(void);
char sendByte(char data);
char readbyte(char ack);

void includerI2C(){
    asm(" include i2c.inc\n");
}
