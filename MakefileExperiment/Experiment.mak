# screeeeeeech
# i don't know what the fuck im doing pls help
# ********************************************************
# some variables or whatever i guess

CC = g++
CFLAGS = -Wall -g

# *********************************************************
# targets? i dont fucking know...

main: main.o Point.o Square.o
	$(CC) $(CFLAGS) -o main main.o Point.o Square.o

main.o: main.cpp Point.h Square.h
	$(CC) $(CFLAGS) -c main.cpp

Point.o: Point.h

Square.o: Square.h Point.h
