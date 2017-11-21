CFLAGS = -ggdb3 -Wall

rainfall: 
	gcc $(CFLAGS) -o rainfall rainfall.c
clean:
	rm *~ *# rainfall
