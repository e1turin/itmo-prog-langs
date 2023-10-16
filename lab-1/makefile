
test: test.o
	ld -o test test.o

%.o: %.asm
	nasm -g -f elf64 -o $@ $<

clean:
	rm *.o

run:
	./test
