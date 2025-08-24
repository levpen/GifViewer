all:
	g++ main.cpp -mwindows -Iinclude -Llib -lraylib -lgdi32 -lwinmm -o gif
