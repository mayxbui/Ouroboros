# https://stackoverflow.com/a/69064732/2912901
ifeq ($(OS),Windows_NT)
    SHELL := powershell.exe
else
   SHELL := powershell
endif
.SHELLFLAGS := -NoProfile -Command 

LIBS = -L./lib -lraylib -lopengl32 -lgdi32 -lwinmm
MAIN = ray_game.cpp

ray:
	g++ -O3 $(MAIN) -o ray -static -I. $(LIBS)
