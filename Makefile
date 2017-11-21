CFLAGS = -ggdb3 -Wall -pedantic

rainfall: 
	gcc $(CFLAGS) -o rainfall rainfall.c
clean:
	rm *~ *# rainfall
