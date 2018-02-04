main: main.c
	gcc -g -o gol.o main.c -I /Library/Frameworks/SDL2.framework/Headers/ -I /Library/Frameworks/SDL2_ttf.framework/Headers/ -framework SDL2 -framework SDL2_ttf