@echo off
cls
gcc -I ../include -L ../lib -o bin/main src/main.c src/structs/*.c -lmingw32 -lSDL2main -lSDL2