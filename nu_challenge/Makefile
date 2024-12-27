all: test1 test2 test3 shell attack

clean:
	rm test1 test2 test3 shell attack

test1:
	gcc -g -static -O0 -z execstack -no-pie -fno-stack-protector test1.c -o test1

test2:
	gcc -g -static -O0 -z execstack -no-pie -fno-stack-protector test2.c -o test2

test3:
	gcc -g -static -O0 -z execstack -no-pie -fno-stack-protector test3.c -o test3

shell:
	gcc -g -static -O0 -z execstack -no-pie -fno-stack-protector shell.c -o shell
	
attack:
	gcc -g -Wall attack.c -o attack
