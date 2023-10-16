LD         	= ld
ASM        	= nasm
ASMFLAGS   	= -f elf64 -g
.PHONY: clean

main: main.o dict.o lib.o
	$(LD) -o $@ $^

main.o: main.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

main.asm: words.inc lib.inc dict.inc defines.inc

%.o: %.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

clean: 
	$(RM) *.o ./main




