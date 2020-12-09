CC          = g++
CFLAGS      = -std=c++17

lab1: Lab1.cpp Data.cpp Parser.cpp Scanner.cpp Storage.cpp
	@$(CC) -o Lab1 $(CFLAGS) $^
	@Lab1

lab2: Lab2.cpp Data.cpp Parser.cpp Scanner.cpp Storage.cpp
	@$(CC) -o Lab2 $(CFLAGS) $^
	@Lab2

clean:
	del /f /q *.o Lab1.exe Lab2.exe

