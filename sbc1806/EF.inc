	align 8
_TEF1:
	ldi 0		;default is false
	plo R15
	phi R15
	B1 $$skpEF1		;i have to reverse the sense of the external line
	inc R15		;if external line is high, supply a 1
$$skpEF1: 
	cretn
	
_TEF2:
	ldi 0		;default is false
	plo R15
	phi R15
	B2 $$skpEF2		;i have to reverse the sense of the external line
	inc R15		;if external line is high, supply a 1
$$skpEF2: 
	cretn
	
_TEF3:
	ldi 0		;default is false
	plo R15
	phi R15
	B3 $$skpEF3		;i have to reverse the sense of the external line
	inc R15		;if external line is high, supply a 1
$$skpEF3: 
	cretn
	
_TEF4:
	ldi 0		;default is false
	plo R15
	phi R15
	B4 $$skpEF4		;i have to reverse the sense of the external line
	inc R15		;if external line is high, supply a 1
$$skpEF4: 
	cretn
;_EFID: ;EF pin identifier it checks if the target pin is set (goes from 1 to 4)