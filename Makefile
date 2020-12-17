CC          = g++
CFLAGS      = -std=c++17 -DDEBUG

main2: main2.cpp Parser.h Lexer.h
	$(CC) -o main2 $(CFLAGS) main2.cpp

main1: main1.cpp Lexer.h
	$(CC) -o main1 $(CFLAGS) main1.cpp

clean:
	-rm -f *.o main1.exe main2.exe

