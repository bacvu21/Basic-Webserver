CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lws2_32  # Add the Winsock library

# Target executable
TARGET = webserver

# Source files
SRCS = server.c
OBJS = server.o

# Default target
all: $(TARGET)

# Rule to link object files into the target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
