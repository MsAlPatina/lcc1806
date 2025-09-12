
#make a binary and make it for the EEPROM
flash: 
	@echo off
	@lcc -target=xr18CX \
	-Wa,-D,STACKLOC=0xEEEE \
	-Wa,-D,NOFILLBSS=1 \
	-Wa,-D,DATALOC=0x8000 \
	-Wa,-D,CPUSPEED=5000000 \
	-Wa,-i,/home/marta/lcc1806/sbc1806 \
	-Wf,-volatile,-cpu1805 \
	-I/home/marta/lcc1806/sbc1806 \
	-DSCC=2 -DDEBUG=0 $(FILE) -o $(FILE).hex #just run make build FILE=program.c

	@srec_cat -Disable_Sequence_Warnings $(FILE).hex -Intel -offset 0x0000 -Disable_Sequence_Warnings $(FILE).hex -Intel -offset 0x8000 -Disable_Sequence_Warnings $(FILE).hex -Intel -offset 0x10000 -Disable_Sequence_Warnings $(FILE).hex -Intel -offset 0x18000 -crop 0x0000 0x20000 -o flashme.hex -Intel -Disable_Sequence_Warnings -contradictory-bytes warning
	@echo "Made ROM file named 'flashme.hex'"
exec: #make a binary and make it bootloadable #just run make build FILE=program.c
	lcc -target=xr18CX \
	-Wa,-D,STACKLOC=0xFFEE \
	-Wa,-D,NOFILLBSS=1 \
	-Wa,-D,DATALOC=0xE000 \
	-Wa,-D,CODELOC=0x9000 \
	-Wa,-D,CPUSPEED=5000000 \
	-Wa,-i,/home/marta/lcc1806/sbc1806 \
	-Wf,-volatile,-cpu1805 \
	-I/home/marta/lcc1806/sbc1806 \
	-DSCC=2 -DDEBUG=0 $(FILE) -o $(FILE).hex
	srec_cat -disable-sequence-warning $(FILE).hex -Intel -crop 0x9000 0xD000 -offset -0x9000 -o uploadme.bin -Binary
	@echo "file 'uploadme.bin' has been made"
	sh /home/marta/lcc1806/upload.sh uploadme.bin
