CC          = g++
CFLAGS      = -std=c++17
TARGET		= sample

all: main.cpp
	@$(CC) -o $(TARGET) $(CFLAGS) $^
	@$(TARGET)

clean:
	del /f /q *.o $(TARGET)

