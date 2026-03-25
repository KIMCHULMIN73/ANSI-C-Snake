# directions for this Makefile
#
# 1) to build : make build
# 2) to run   : make run
# 3) to clean : make clean


default: build

build:
	gcc -o ./output/snake main.c backend.c frontend.c -lncurses

run: build
	./output/snake

clean:
	rm ./output/snake
