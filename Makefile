default: build

build:
	gcc -o snake src/main.c src/backend.c src/frontend.c -lncurses

run: build
	./snake

clean:
	rm snake
