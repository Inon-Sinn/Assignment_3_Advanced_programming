.PHONY: all 

all: stree 

stree: stree.c
	gcc -o stree stree.c

.PHONY: clean
clean: 
		rm -f *.o *.a *.so stree 