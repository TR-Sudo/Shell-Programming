CFLAGS = -Wall -g

mysh : parse.o exec.o main.o
	cc -o mysh parse.o exec.o main.o

clean :
	rm mysh parse.o exec.o main.o
