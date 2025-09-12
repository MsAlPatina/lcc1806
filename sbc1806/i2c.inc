;I2C lib for the SBC1806

    ;DEC R2
	;LDI 0x01 ;byte to set
	;STR R2
	;out 1

A0_high: macro
    DEC R2
    ldi 1
    str R2
    out 1
    endm

A0_low: macro
    DEC R2
    ldi 0
    str R2
    out 1
    endm


_initializei2c:

    A0_high
    DEC R2
    ldi 0x80
    str R2
    out 5
    A0_low
    DEC R2
    ldi 0x55
    str R2
    out 5
    A0_high
    DEC R2
    ldi 0xA0
    str R2
    out 5
    A0_low
    DEC R2
    ldi 0x1C
    str R2
    out 5
    A0_high
    DEC R2
    ldi 0xC1
    str R2
    out 5
    Cretn ;done with initializing


_stopI2C:

    A0_high
    DEC R2
    ldi 0xC3
    str R2
    out 5
    Cretn ;thats litrally it

_sendByte:

    A0_high

$$waitbeforesend:
    inp 5
    ANI 0x80
    LBNZ $$waitbeforesend

    A0_low

    DEC R2
    GLO R12
    str R2
    out 5 ;send data

    A0_high
$$waitaftersend:
    inp 5
    ANI 0x80
    LBNZ $$waitaftersend

    inp 5
    ANI 0x08 ;check ACK/NACK
    PLO R15 ;return its state
    Cretn


_readByte:

    A0_high
$$waitbeforereceive:
    inp 5
    ANI 0x80
    LBNZ $$waitbeforereceive


    GLO R12 ;if we send ACK/NACK
    LBZ $$sendack
    DEC R2
    ldi 0x44
    str R2
    out 5
$$sendack:
    A0_low
    inp 5 ;read data
    PLO R15 ;return it
    Cretn

_startI2C:

    GLO R13;if its Read or Write

    LBNZ $$startread

    ;if write

    GLO R12
    SHL ;shift left and LSB becomes 0
    PLO R12 ;save it
    LBR $$mainsend

$$startread
    GLO R12
    SHL ;shift left and LSB becomes 0
    ORI 0x01
    PLO R12

$$mainsend
    A0_low
    
    DEC R2
    GLO R12
    str R2
    out 5 ;send the address

    A0_high

    DEC R2
    LDI 0xC5 ;start I2C
    str R2
    out 5 

$$startwait:
    inp 5
    ANI 0x80
    LBNZ $$startwait

    inp 5
    ANI 0x08
    LBZ $$gotACK
    ;if no acknowladge return 1
    DEC R2
    ldi 0xC3
    str R2
    out 5 ;stop the I2C

    LDI 0x01
    PLO R15
    Cretn ;leave

$$gotACK: ;if acknowladged return 0

    LDI 0x00
    PLO R15

    ;before we done we need to handle if its a Read or Write

    GLO R13

    BZ $$finishwrite
    ;if we reading
    A0_low
    inp 5 ;dummy read


$$finishwrite:

    Cretn ;leave



