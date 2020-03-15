DEBUG=no

assembler:
	make -C ./assembler DEBUG=$(DEBUG)

vm:
	make -C ./vm DEBUG=$(DEBUG)

all: assembler vm
