CC=g++
CXX_FLAGS=-std=c++14 -ggdb
TARGET=out

all:
	$(CC) test.cpp $(CXX_FLAGS) -o $(TARGET)

clean:
	rm $(TARGET) test *.ppm 2> /dev/null > /dev/null || true

