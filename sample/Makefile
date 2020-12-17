CC          = g++
CFLAGS      = -std=c++17

main2: main2.cpp Parser.h Lexer.h
	$(CC) -o main2 $(CFLAGS) main2.cpp
	./main2 test/test4.in

main1: main1.cpp Lexer.h
	$(CC) -o main1 $(CFLAGS) main1.cpp

clean:
	-rm -f *.o main1.exe main2.exe

