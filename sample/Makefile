CC          = g++
CFLAGS      = -std=c++17
TARGET		= sample

all: main.cpp
	@$(CC) -o $(TARGET) $(CFLAGS) $^
	@$(TARGET) test/test2.in

clean:
	del /f /q *.o $(TARGET)

