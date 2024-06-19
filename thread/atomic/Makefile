cmp_and_swap:
	nasm -f elf64 cmp_and_swap.asm -o cmp_and_swap.o
	g++ -o cmp_and_swap cmp_and_swap.cpp cmp_and_swap.o
cmp_and_swap2:
	nasm -f elf64 cmp_and_swap.asm -o cmp_and_swap.o
	g++ -o cmp_and_swap2 cmp_and_swap2.cpp cmp_and_swap.o -lpthread


clean:
	rm *.o cmp_and_swap cmp_and_swap2