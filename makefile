task1:
	gcc -g -Wall -m32 -o task1 task1.c

clean1:
	rm task1

task4:
	gcc -g -Wall -m32 -fno-pie -fno-stack-protector -o digit_counter task4.c

clean4:
	rm digit_counter