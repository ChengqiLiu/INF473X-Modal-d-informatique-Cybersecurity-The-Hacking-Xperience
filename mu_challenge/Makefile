all: terminal

clean:
	rm reverse_terminal

terminal:
	gcc -g -static -O0 -z execstack -no-pie -fno-stack-protector reverse_terminal.c -o reverse_terminal

