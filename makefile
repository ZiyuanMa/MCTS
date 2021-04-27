all: test.cpp reversi.cpp
	g++ -std=c++17 test.cpp reversi.cpp -o test.o
clean:
	rm -f test.o