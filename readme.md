
# sudoku
This is a sudoku solver. It solves a sudoku by representing it as a graph, where each node is a square on the board and is connected to all of the nodes in the same row, column or section, and then attempting to [color](https://en.wikipedia.org/wiki/Graph_coloring) the nodes of that graph.

## Build
You will need GNU make
```sh
git clone https://github.com/AjayMT/sudoku.git
cd sudoku
make
```

## Usage
```
sudoku <size>
```
`size` is the length of one side of the puzzle board. The program reads input from stdin, formatted as follows:
```
- - 2 - - - - 7 -
- - - - 9 5 - - 3
- - 9 2 - - - - 1
8 - 6 - - 1 - - -
- 4 - - 6 - - 8 -
- - - 3 - - 6 - 9
2 - - - - 7 1 - -
5 - - 4 1 - - - -
- 9 - - - - 8 - -
```
Here, pre-solved/hint squares have a number and unsolved squares are dashes. Each row is a set of at least `size` space-separated numbers or dashes, and there must be at least `size` rows in the board. The program outputs its solution in the same format.

For example to solve `inputs/input` and output the solution to the console:
```sh
sudoku 9 < inputs/input
```
