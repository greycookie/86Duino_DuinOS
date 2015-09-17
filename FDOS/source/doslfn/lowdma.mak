all: com sys

sys: lowdma.sys

com: lowdma.com

lowdma.sys: lowdma.asm
	tasm lowdma.asm 
	tlink -x lowdma.obj  
	del lowdma.obj
	exe2bin lowdma.exe lowdma.sys 
	del lowdma.exe

lowdma.com: lowdma.asm
	tasm -DMAKECOM lowdma.asm
	tlink -t -x lowdma.obj
	del lowdma.obj
