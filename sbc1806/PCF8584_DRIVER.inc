;PCF8584_DRIVER.INC

 IFNDEF PCF_IO
PCF_IO EQU 	5
 ENDIF

_PCF_INIT:

	;assume by default for A0 of PCF8584 is wired to IO_A0
	
	
	;set A0=1- access S1
	LDI 0x01
	STR R2
	OUT 1
	DEC R2
	;--------
	
	;soft reset
	LDI 0x80 
	STR R2
	OUT PCF_IO
	DEC R2
	;----------
	
	;set A0=0
	LDI 0x00
	STR R2
	OUT 1
	DEC R2
	;--------
	
	;set own address
	LDI 0x55
	STR R2
	OUT PCF_IO
	DEC R2
	;---------------
	
	;set A0=1
	LDI 0x01
	STR R2
	OUT 1
	DEC R2
	;--------
	
	;clear status and ES1 is set
	LDI 0xA0
	STR R2
	OUT PCF_IO
	DEC R2
	;-------------------------------------
	;set A0=0
	LDI 0x00
	STR R2
	OUT 1
	DEC R2
	;--------
	;at clock register
	LDI 0x1C
	STR R2
	OUT PCF_IO
	DEC R2
	;----------------
	
	;set A0=1
	LDI 0x01
	STR R2
	OUT 1
	DEC R2
	;--------
	
	LDI 0xC1
	STR R2
	OUT PCF_IO
	DEC R2
	
	
	Cretn
	
	; align 8
	
; _I2C_send:

	
	; ;R12 is data
	
	; ; ;set A0=1-access S1
	; ; LDI 0x01
	; ; STR R2
	; ; OUT 1
	; ; DEC R2
	; ; ;--------
	
	; ;$$checktransmission332333:
	; ;SEQ	;debug
	; ;INP 7
	; ;ANI 0x80 ;check if transmission ended
	; ;BNZ $$checktransmission332333
	
	; ;$$checktransmission:
	; ;SEQ	;debug
	; ;INP 7
	; ;ANI 0x01 ;check if busy
	; ;BZ $$checktransmission
	
	; ;set A0=0 S0
	; LDI 0x00
	; STR R2
	; OUT 1
	; DEC R2
	; ;--------
	
	; GLO R12
	; STR R2
	; OUT PCF_IO
	; DEC R2
	
; $$checktransmission3:
	; ;SEQ	;debug
	; INP PCF_IO
	; ANI 0x80 ;check if transmission ended
	; BNZ $$checktransmission3
	; ;REQ	;debug
	; INP PCF_IO
	; ANI 0x08 ;check for ACK
	; BNZ $$stopTX2 ;if NACK stop transmission right now
	
	; LDI 0x01
	; PLO R15
	; LDI 0x00
	; PHI R15
	; Cretn
; $$stopTX2
	
	; LDI 0x00
	; PLO R15
	; LDI 0x00
	; PHI R15
	; Cretn
	

; _I2C_request:
	; ;R12 has the address
	; ;R15 has the data
	
	; ;set A0=1-access S1
	; LDI 0x01
	; STR R2
	; OUT 1
	; DEC R2
	; ;--------

	; LDI 0xC0
	; STR R2
	; OUT PCF_IO
	; DEC R2
	
	
; $$isI2Cbusy4:
	
	; INP PCF_IO
	; ;SEQ	;debug ;JAM
	; ANI 0x01 ;test for !BB ;JAM
	; LBZ $$isI2Cbusy4 ;wait untill bus is ready
	; ;REQ	;debug
	; ;set A0=0 S0
	; LDI 0x00
	; STR R2
	; OUT 1
	; DEC R2
	; ;--------
	
	; ;form address
	; GLO R12
	; SHL ;shift it left, LSB becomes 0, write operation
	; ORI 0x01;make it a read operation
	; STR R2
	; OUT PCF_IO
	; DEC R2
	; ;------------
	
	; ;set A0=1 S1
	; LDI 0x01
	; STR R2
	; OUT 1
	; DEC R2
	; ;--------
	
	; ;generate start condition and send slave address
	; LDI 0xC4
	; STR R2
	; OUT PCF_IO
	; DEC R2
	
	; ;set A0=0 S0
	; LDI 0x00
	; STR R2
	; OUT 1
	; DEC R2
	; ;--------
	; INP PCF_IO
	; ;set A0=1 S1
	; LDI 0x01
	; STR R2
	; OUT 1
	; DEC R2
	; ;--------
	
	
	; LDI 0x40
	; STR R2
	; OUT PCF_IO
	; DEC R2
	; ;------------
	
; $$checktransmission66:
	; ;SEQ	;debug
	; INP PCF_IO
	; ANI 0x80 ;check if transmission ended
	; BNZ $$checktransmission66
	
	; LDI 0xC3
	; STR R2
	; OUT PCF_IO
	; DEC R2
	
	; ;set A0=0 S0
	; LDI 0x00
	; STR R2
	; OUT 1
	; DEC R2
	; ;--------
	; INP PCF_IO
	; PLO R15
	; LDI 0x00
	; PHI R15

	
	
	; ; ;REQ	;debug
	; ; INP 7
	; ; ANI 0x08 ;check for ACK
	; ; BNZ $$stopTX66 ;if NACK stop transmission right now
	

	; ; ; Cretn
; ; $$stopTX66
	; ; LDI 0x01
	; ; PLO R15
	; ; LDI 0x00
	; ; PHI R15	
	
	; Cretn


; _I2C_read:
	
	
	; ; ;R15 is data

	; ;set A0=0
	; LDI 0x00
	; STR R2
	; OUT 1
	; DEC R2
	; ;--------
	
	
	
	; INP PCF_IO ;actual read
	
	; PLO R15
	; LDI 0x00
	; PHI R15
	



	; Cretn
	; ; ;set A0=1-access S1
	; ; LDI 0x01
	; ; STR R2
	; ; OUT 1
	; ; DEC R2
	; ; ;--------

	; ; LDI 0xC0
	; ; STR R2
	; ; OUT 7
	; ; DEC R2
	
	
; ; $$isI2Cbusy_RX:
	
	; ; INP 7
	; ; ;SEQ	;debug ;JAM
	; ; ANI 0x01 ;test for !BB ;JAM
	; ; LBZ $$isI2Cbusy_RX ;wait untill bus is ready
	; ; ;REQ	;debug
	; ; ;set A0=0 S0
	; ; LDI 0x00
	; ; STR R2
	; ; OUT 1
	; ; DEC R2
	; ; ;--------
	
	; ; ;form address
	; ; GLO R12
	; ; SHL ;shift it left, LSB becomes 0
	; ; ORI 0x01 ;turn LSB to 1, read operation
	; ; STR R2
	; ; OUT 7
	; ; DEC R2
	; ; ;------------
	
	; ; ;set A0=1 S1
	; ; LDI 0x01
	; ; STR R2
	; ; OUT 1
	; ; DEC R2
	; ; ;--------
	
	; ; ;generate start condition and send slave address
	; ; LDI 0xC5
	; ; STR R2
	; ; OUT 7
	; ; DEC R2
	
; ; $$checktransmission_RX:
	; ; ;SEQ	;debug
	; ; INP 7
	; ; ANI 0x80 ;check if transmission ended
	; ; BNZ $$checktransmission_RX
	
	
	; ; ;send NACK
	; ; LDI 0x40
	; ; STR R2
	; ; OUT 7
	; ; DEC R2
	; ; ;---------
	
	
	; ; ;set A0=0
	; ; LDI 0x00
	; ; STR R2
	; ; OUT 1
	; ; DEC R2
	; ; ;--------
	
	; ; INP 7 ;dummy read
	
	
	
	; ; ;set A0=1
	; ; LDI 0x01
	; ; STR R2
	; ; OUT 1
	; ; DEC R2
	; ; ;--------
	
; ; $$checkendoftransmission_RX:
	; ; ;SEQ	;debug
	; ; INP 7
	; ; ANI 0x80 ;check if transmission ended
	; ; BNZ $$checkendoftransmission_RX
	; ; ;REQ	;debug
	
; ; $$stop_RX:
	; ; ;set A0=1
	; ; LDI 0x01
	; ; STR R2
	; ; OUT 1
	; ; DEC R2
	; ; ;--------
	
	; ; LDI 0xC3
	; ; STR R2
	; ; OUT 7
	; ; DEC R2
	
	; ; ;set A0=0
	; ; LDI 0x00
	; ; STR R2
	; ; OUT 1
	; ; DEC R2
	; ; ;--------
	
	; ; INP 7 ;read actual data
	; ; PLO R15 ;return it.
	
	; ; LDI 0x00
	; ; PHI R15
	
	; ; Cretn
	
	
; _I2C_beginTransmission:

	; ;R12 has the address
	; ;R15 returns 1 if ACK, else NAK(0)

	
	
	; ;set A0=1-access S1
	; LDI 0x01
	; STR R2
	; OUT 1
	; DEC R2
	; ;--------

	; LDI 0xC0
	; STR R2
	; OUT PCF_IO
	; DEC R2
	
	
; $$isI2Cbusy:
	
	; INP PCF_IO
	; ;SEQ	;debug ;JAM
	; ANI 0x01 ;test for !BB ;JAM
	; LBZ $$isI2Cbusy ;wait untill bus is ready
	; ;REQ	;debug
	; ;set A0=0 S0
	; LDI 0x00
	; STR R2
	; OUT 1
	; DEC R2
	; ;--------
	
	; ;form address
	; GLO R12
	; SHL ;shift it left, LSB becomes 0, write operation
	; STR R2
	; OUT PCF_IO
	; DEC R2
	; ;------------
	
	; ;set A0=1 S1
	; LDI 0x01
	; STR R2
	; OUT 1
	; DEC R2
	; ;--------
	
	; ;generate start condition and send slave address
	; LDI 0xC5
	; STR R2
	; OUT PCF_IO
	; DEC R2
	
; $$checktransmission:
	; ;SEQ	;debug
	; INP 7
	; ANI 0x80 ;check if transmission ended
	; BNZ $$checktransmission
	; ;REQ	;debug
	; INP 7
	; ANI 0x08 ;check for ACK
	; BNZ $$stopTX ;if NACK stop transmission right now
	
	; LDI 0x01
	; PLO R15
	; LDI 0x00
	; PHI R15
	
	; Cretn
	
; $$stopTX:
	
	; Ccall _I2C_endTransmission
	
	; ; ;set A0=1
	; ; LDI 0x01
	; ; STR R2
	; ; OUT 1
	; ; DEC R2
	; ; ;--------
	
	; ; LDI 0xC3
	; ; STR R2
	; ; OUT 7
	; ; DEC R2
	
	; LDI 0x00
	; PLO R15
	; LDI 0x00
	; PHI R15
	
	; Cretn
	


; _I2C_endTransmission:

	; ;set A0=1
	; LDI 0x01
	; STR R2
	; OUT 1
	; DEC R2
	; ;--------
	
	; LDI 0xC3
	; STR R2
	; OUT PCF_IO
	; DEC R2
	
	; Cretn
