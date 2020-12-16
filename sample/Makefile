CC          = g++
CFLAGS      = -std=c++17

main1: main1.cpp Data.cpp Parser.cpp Lexer.cpp
	@$(CC) -o main1 $(CFLAGS) $^
	@chcp 65001
	@main1

main2: main2.cpp Data.cpp Parser.cpp Lexer.cpp
	@$(CC) -o main2 $(CFLAGS) $^
	@chcp 65001
	@main2

clean:
	del /f /q *.o main1.exe main2.exe

