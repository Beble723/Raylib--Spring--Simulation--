# Compiler
CC = g++

# Compiler flags
CFLAGS = -std=c++14 -Wall -O2

# Include paths 
INCLUDE_PATHS = -I/usr/include -I./src -I./include

# paths to raylib
LDFLAGS = -L/usr/lib

# links raylib
LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11


SRC = $(wildcard src/*.cpp) #reads everything in src folder

# Object names .cpp files
OBJ = $(SRC:.cpp=.o)

# Output executable name
OUTPUT = game

# Build executable name
all: $(OUTPUT)

#  build the executable from object files
$(OUTPUT): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(OUTPUT) $(LDFLAGS) $(LDLIBS)

# compiles c++ into object files
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Clean everything
clean:
	rm -f $(OBJ) $(OUTPUT)

# Phony targets
.PHONY: all clean

