CC=g++
CXX_FLAGS=-std=c++14 -O3
TARGET=main

all:
	$(CC) main.cpp $(CXX_FLAGS) -o $(TARGET)

build:
	make clean && make && ./$(TARGET) >> image.ppm && open image.ppm

clean:
	rm $(TARGET) test *.ppm 2> /dev/null > /dev/null || true

.PHONY: test
