
sudoku: sudoku.cpp
	c++ sudoku.cpp -o sudoku -std=c++17

.PHONY: clean
clean:
	rm -f sudoku
