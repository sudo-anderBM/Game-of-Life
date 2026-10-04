# Makefile - Proyecto Game of Life (animacion con lista doblemente ligada)

CXX = g++
CXXFLAGS = -Wall -std=c++17
LIBS = $(shell pkg-config --libs raylib)
INCLUDES = $(shell pkg-config --cflags raylib)

SRC = src/main.cpp
TARGET = animacion

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)