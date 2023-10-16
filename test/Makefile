run: main
	./main

main: main.o
	ld -o $@ $<

%.o: %.asm
	nasm -g -f elf64 -o $@ $<
