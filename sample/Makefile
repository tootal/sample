CC          = g++
CFLAGS      = -std=c++17

main2: main2.cpp
	@$(CC) -o main2 $(CFLAGS) $^
	@chcp 65001
	@main2

main1: main1.cpp
	@$(CC) -o main1 $(CFLAGS) $^
	@chcp 65001
	@main1

clean:
	del /f /q *.o main1.exe main2.exe

