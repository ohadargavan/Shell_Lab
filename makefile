all: myshell

myshell: myshell.c
	gcc -o myshell myshell.c

looper: looper.c
	gcc -o looper looper.c

lineParser: LineParser.c
	gcc -o LineParser LineParser.c
	
clean:
	rm -f myshell looper LineParser