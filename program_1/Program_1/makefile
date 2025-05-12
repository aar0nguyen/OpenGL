# Graphics Prg 1 for Aaron Nguyen

######################################################
# makefile

# This makefile was used to compile and link libraries and file together.
# The libraries includes GLEW, GLUT, and OpenGL programs that are used within
# the header files OpenGL445Setup-aug24.h and drawing.h and source file 
# source.cpp. The makefile also generates the source.exe file that will be used
# to execute the source.cpp operations and generate the canvas using OpenGL programs.

######################################################

# Variables
CXX = g++
CXXFLAGS = -Wall -lGL -lGLU -lglut -lGLEW
TARGET = source.exe
SRC = source.cpp
HEADER = OpenGL445Setup-aug24.h drawing.h


# Default rule
all: $(TARGET)

# Linking and compiling
$(TARGET): $(SRC) $(HEADER)
	$(CXX) $(SRC) -o $(TARGET) \
	$(CXXFLAGS)

# Clean up
clean:
	rm -f $(TARGET)
