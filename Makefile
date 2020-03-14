DEBUG=no

all:
	make -C ./assembler DEBUG=$(DEBUG)
	
	make -C ./vm DEBUG=$(DEBUG)