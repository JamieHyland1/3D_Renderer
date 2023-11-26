build:
	gcc -o renderer -Wall -std=c99 ./src/*.c -IC:/SDL2/include -lSDL2main -lm -lSDL2 -LC:/SDL2/lib/x64
debug-build:
	gcc -o renderer -g -fno-stack-protector -z execstack -Wall -std=c99 ./src/*.c -IC:/SDL2/include -lSDL2main -lm -lSDL2 -LC:/SDL2/lib/x64 
debug:
	gdb  renderer.exe 
run:
	./renderer
clean:
	del renderer.exe