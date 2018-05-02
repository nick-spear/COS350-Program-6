#    COS350 Program #6: linecount
#    Author: Nickolas Spear
#
#    Makefile

# Force recompile all
all: clean lc1 lc2 lc3

# Force recompile all with optimize flag
allopt: clean lc1opt lc2opt lc3opt

clean:
	rm -f lc1 lc2 lc3

lc1:
	gcc lc1.c -o lc1

lc2:
	gcc lc2.c -o lc2

lc3:
	gcc -pthread lc3.c -o lc3

lc1opt:
	gcc -O2 lc1.c -o lc1

lc2opt:
	gcc -O2 lc2.c -o lc2

lc3opt:
	gcc -O2 -pthread lc3.c -o lc3
