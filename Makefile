# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra

# Source files
SOURCE = find.c

# Executable name
EXECUTABLE = find

# Default target
all: $(EXECUTABLE)

# Target: $(EXECUTABLE)
$(EXECUTABLE): $(SOURCE)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCE)

# Clean up generated files
clean:
	rm -f $(EXECUTABLE)
