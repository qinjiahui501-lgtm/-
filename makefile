CC = gcc
CFLAGS = -Wall -g
TARGET = minibox

all: $(TARGET)

$(TARGET): minifilebox.c
	$(CC) $(CFLAGS) -o $(TARGET) minifilebox.c
clean:
	rm -f $(TARGET) *.o