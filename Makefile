CC := g++
DEBUG := -g -fsanitize=address
CCFLAGS := -Wall -Wpedantic -Wextra -Wconversion $(DEBUG)
LDFLAGS := $(DEBUG)

perceptron: Matrix.o newMain.o RANet.o
	$(CC) $(LDFLAGS) newMain.o Matrix.o RANet.o -o perceptron

Matrix.o: Matrix.cpp Matrix.hpp
	$(CC) -std=c++2b $(CCFLAGS) Matrix.cpp -c

newMain.o: newMain.cpp
	$(CC) -std=c++2b $(CCFLAGS) newMain.cpp -c

RANet.o: RANet.cpp RANet.hpp
	$(CC) -std=c++2b $(CCFLAGS) RANet.cpp -c

.PHONY: clean
clean: 
	rm -f ./*.o