CFLAGS = -O2 -Wall
MTFLAGS = -O2 -Wall
CC = gcc
LIB = -lpthread

rainfall: 
	$(CC) $(CFLAGS) -o rainfall rainfall.c

mt_rainfall:
	$(CC) $(MTFLAGS) -o mt_rainfall mt_rainfall.c $(LIB)

clean:
	rm *~ *# rainfall mt_rainfall
