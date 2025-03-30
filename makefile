# Makefile

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Executable name
TARGET = a.out

# Source files
SRCS = hop.c input.c display.c main.c log.c reveal.c seek.c proclore.c syscom.c io.c pipe.c activites.c signal.c iman.c Neonate.c processes.c aliases.c

# Object files (replace .c with .o for each source file)
OBJS = $(SRCS:.c=.o)

# Default target (first target in the file)
all: $(TARGET)

# Link object files to create the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile .c files to .o object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up compiled files
clean:
	rm -f $(OBJS) $(TARGET)
