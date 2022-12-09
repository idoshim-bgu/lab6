all: myshell

myshell: myshell.o LineParser.o
	gcc -m32 -g -Wall -o myshell myshell.o LineParser.o

myshell.o: myshell.c LineParser.h
	gcc -g -Wall -m32 -c -o myshell.o myshell.c

LineParser.o: LineParser.c LineParser.h
	gcc -g -Wall -m32 -c -o LineParser.o LineParser.c

.PHONY: clean
clean:
	rm -f *.o myshell