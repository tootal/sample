CC          = g++
CFLAGS      = -std=c++17

main2: main2.cpp Data.cpp Parser.cpp Lexer.cpp Storage.cpp
	@$(CC) -o main2 $(CFLAGS) $^
	@main2

main1: main1.cpp Data.cpp Parser.cpp Lexer.cpp Storage.cpp
	@$(CC) -o main1 $(CFLAGS) $^
	@main1

clean:
	del /f /q *.o main1.exe main2.exe

