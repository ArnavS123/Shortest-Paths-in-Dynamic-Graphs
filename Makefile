CC = gcc
CFLAGS = -std=c11 -g -O3 -lm
SRCS = Shortest-Paths-Graph.c
OBJS = $(SRCS:.c=.o)
TARGET = pa3

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

test: $(TARGET)
	./$(TARGET) graph.txt

clean:
	rm -f $(TARGET) $(OBJS) *~
