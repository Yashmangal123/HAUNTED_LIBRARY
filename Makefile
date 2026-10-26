CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra
TARGET = haunted

SRCS = main.c \
       login.c \
       auth.c \
       player.c \
       rooms.c \
       puzzles.c \
       save.c \
       util.c \
       ui.c

OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe

rebuild: clean $(TARGET)
