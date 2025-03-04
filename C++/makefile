CC = g++
CFLAGS = -Wall -g
TARGET = my_shell
OBJS = my_shell.o my_ls.o my_echo.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

my_shell.o: my_shell.cpp
	$(CC) $(CFLAGS) -c my_shell.cpp

my_ls.o: my_ls.cpp
	$(CC) $(CFLAGS) -c my_ls.cpp

my_echo.o: my_echo.cpp
	$(CC) $(CFLAGS) -c my_echo.cpp

clean:
	rm -f $(OBJS) $(TARGET)
