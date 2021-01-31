CC = gcc
CFLAG = -Wall -Werror -Og

tlb.svg: plot.py tlb.data
	./plot.py

tlb.data: tlb.sh tlb
	./tlb.sh > tlb.data

tlb: tlb.c
	$(CC) $(CFLAG) -o tlb tlb.c

clean:
	rm tlb.svg tlb.data tlb
