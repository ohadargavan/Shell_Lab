all: myshell looper mypipe

myshell: myshell.c LineParser.c LineParser.h
	gcc -o myshell myshell.c LineParser.c

looper: looper.c
	gcc -o looper looper.c

mypipe: mypipe.c
	gcc -o mypipe mypipe.c

clean:
	rm -f myshell looper LineParser mypipe