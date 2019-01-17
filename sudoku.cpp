
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <cmath>
#include <sstream>
#include <iterator>
#include <cstdlib>


// a `node` is a single square on the board. row, column and block are vectors
// of pointers to the other nodes in the same row, column and block respectively.
// neighbours is the union of row, column and block, i.e all the nodes that this
// node cannot share a number with. labels is a map containing all the possible
// labels (i.e numbers) for this node. a node is solved when it has only one possible
// label
struct node
{
  std::vector<std::weak_ptr<node>> neighbours;
  std::vector<std::weak_ptr<node>> row;
  std::vector<std::weak_ptr<node>> column;
  std::vector<std::weak_ptr<node>> block;
  std::unordered_map<int, bool> labels;
  bool solved = false;

  bool set (int label);
  bool update (int label);
  bool check_unique (int label, std::vector<std::weak_ptr<node>>& group);
};

// this function solves a node by reducing its 'labels' map to just one label
// and updating its solved state. upon being solved, every node updates all of its
// neighbours. this function returns false if setting the node's label resulted
// in an invalid configuration, and true otherwise.
bool node::set (int label)
{
  this->solved = true;
  this->labels.clear();
  this->labels[label] = true;

  for (auto n : this->neighbours)
    if (!(std::shared_ptr<node>(n)->update(label))) return false;

  return true;
}

// this function updates a node's set of possible labels (and can therefore update
// its solved state) every time one of its neighbours is solved. if a node has only
// one possible label or has a unique label among its row/column/block (see check_unique),
// node::set is called as well, which updates the node's solved state. this function
// returns false if updating the node results in an invalid configuration, and true
// otherwise.
bool node::update (int label)
{
  if (this->solved) return label != this->labels.begin()->first;

  this->labels.erase(label);

  bool success = this->check_unique(label, this->row);
  if (this->solved) return success;
  success = this->check_unique(label, this->column);
  if (this->solved) return success;
  success = this->check_unique(label, this->block);
  if (this->solved) return success;

  if (this->labels.size() == 1)
    return this->set(this->labels.begin()->first);

  return true;
}

// this function checks if a node has a label that is unique among nodes within
// a particular group (i.e row/column/block). if so, node::set is called and the
// node is 'solved'. this function return false if the node is solved but setting
// its label results in an invalid configuration, and true otherwise.
bool node::check_unique (int label, std::vector<std::weak_ptr<node>>& group)
{
  std::unordered_map<int, bool> glabels;
  for (auto n : group)
    for (auto p : std::shared_ptr<node>(n)->labels)
      glabels[p.first] = true;

  for (auto l : this->labels)
    if (!glabels[l.first])
      return this->set(l.first);

  return true;
}


// this function constructs the board and the graph of nodes. 'size' is
// the length of one side of the square board, so the board vector that is
// returned has size*size nodes in it. when constructing the board, this function
// also popoulates each nodes's row, column, block and neighbours vectors, and
// initializes its set of labels.
std::vector<std::shared_ptr<node>> make_board (int size)
{
  std::vector<std::shared_ptr<node>> board;
  int blksize = (int)std::sqrt(size);

  for (int i = 0; i < (size * size); ++i) {
    int x = i % size;
    int y = i / size;
    auto current = std::make_shared<node>();

    for (int j = 1; j <= size; ++j) current->labels[j] = true;
    for (int k = 0; k < board.size(); ++k) {
      int kx = k % size;
      int ky = k / size;
      bool sameblkx = (x / blksize) == (kx / blksize);
      bool sameblky = (y / blksize) == (ky / blksize);
      bool sameblk = sameblkx && sameblky;
      bool samerow = y == ky;
      bool samecol = x == kx;
      if (sameblk || samerow || samecol) {
        auto wp_current = std::weak_ptr<node>(current);
        auto wp_boardk = std::weak_ptr<node>(board[k]);

        if (samerow) {
          board[k]->row.push_back(wp_current);
          current->row.push_back(wp_boardk);
        }
        if (samecol) {
          board[k]->column.push_back(wp_current);
          current->column.push_back(wp_boardk);
        }
        if (sameblk) {
          board[k]->block.push_back(wp_current);
          current->block.push_back(wp_boardk);
        }

        board[k]->neighbours.push_back(wp_current);
        current->neighbours.push_back(wp_boardk);
      }
    }

    board.push_back(current);
  }

  return board;
}


// this function constructs a string representation of a board. unsolved nodes
// are represented as "-"
std::string format_board (std::vector<std::shared_ptr<node>>& board)
{
  std::ostringstream str;
  int size = (int)std::sqrt(board.size());
  for (int x = 0; x < size; ++x) {
    for (int y = 0; y < size; ++y) {
      int index = (x * size) + y;
      if (board[index]->solved)
        str << board[index]->labels.begin()->first;
      else str << "-";
      str << " ";
    }

    str << std::endl;
  }

  return str.str();
}


// this function sets the labels of the pre-solved nodes in a board by reading
// an input string with numbers and "-". see "inputs/" for example input strings
void set_board (std::vector<std::shared_ptr<node>>& board, std::string input)
{
  int size = (int)std::sqrt(board.size());
  std::stringstream inputstream(input);
  std::string line;
  for (int x = 0; x < size && std::getline(inputstream, line, '\n'); ++x) {
    std::stringstream linestream(line);
    std::string token;
    for (int y = 0; y < size && std::getline(linestream, token, ' '); ++y) {
      if (token == "-") continue;
      int num = std::stoi(token);
      int index = (x * size) + y;
      board[index]->set(num);
    }
  }
}


// this function returns a pointer to the first unsolved node in a board
std::shared_ptr<node> unsolved_node (std::vector<std::shared_ptr<node>>& board)
{
  for (auto n : board)
    if (!(n->solved)) return n;

  return nullptr;
}


// this function copies all of the state (i.e node labels) in a board into a
// vector of pairs. each pair contains a map (for labels) and boolean (for 'solved'
// boolean)
std::vector<std::pair<std::unordered_map<int, bool>, bool>>
capture_board (std::vector<std::shared_ptr<node>>& board)
{
  std::vector<std::pair<std::unordered_map<int, bool>, bool>> captured;
  captured.reserve(board.size());
  for (auto n : board) {
    auto p = std::make_pair(n->labels, n->solved);
    captured.push_back(p);
  }

  return captured;
}


// this function restores a board to a captured state (see capture_board above)
void restore_board (std::vector<std::shared_ptr<node>>& board,
                    std::vector<std::pair<std::unordered_map<int, bool>, bool>>& captured)
{
  for (int i = 0; i < board.size(); ++i) {
    board[i]->labels = captured[i].first;
    board[i]->solved = captured[i].second;
  }
}


// this function applies a standard depth-first recursive algorithm to a board. this is
// necessary when nodes cannot be solved by the "set -> update -> check_unique" graph
// coloring mechanism
bool bruteforce_board (std::vector<std::shared_ptr<node>>& board, std::shared_ptr<node> u)
{
  if (u == nullptr) return true;

  auto labels = u->labels;
  for (auto l : labels) {
    auto captured = capture_board(board);
    bool success = u->set(l.first);
    if (success && bruteforce_board(board, unsolved_node(board)))
      return true;
    restore_board(board, captured);
  }

  return false;
}


int main (int argc, char *argv[])
{
  if (argc != 2) {
    std::cout << "Usage: sudoku <size>" << std::endl;
    return 1;
  }

  int size = std::atoi(argv[1]);
  auto board = make_board(size);

  std::istreambuf_iterator<char> stdin_begin(std::cin), stdin_end;
  std::string input(stdin_begin, stdin_end);

  set_board(board, input);

  // brute force :(
  bruteforce_board(board, unsolved_node(board));

  std::cout << format_board(board);

  return 0;
}
