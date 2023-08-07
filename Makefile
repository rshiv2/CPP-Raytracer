CC=g++
CXX_FLAGS=-std=c++14 -ggdb
TARGET=main

all:
	$(CC) main.cpp $(CXX_FLAGS) -o $(TARGET)

test:
	$(CC) test.cpp $(CXX_FLAGS) -o test

build:
	make clean && make && ./$(TARGET) >> image.ppm && open image.ppm

clean:
	rm $(TARGET) test *.ppm 2> /dev/null > /dev/null || true

.PHONY: test
