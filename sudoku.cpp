
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <cmath>
#include <sstream>
#include <iterator>
#include <cstdlib>


struct node
{
  std::vector<std::shared_ptr<node>> neighbours;
  std::vector<std::shared_ptr<node>> row;
  std::vector<std::shared_ptr<node>> column;
  std::vector<std::shared_ptr<node>> block;
  std::unordered_map<int, bool> labels;
  bool solved = false;

  void set (int label);
  void update (int label);
  bool check_unique (int label, std::vector<std::shared_ptr<node>> group);
};

void node::set (int label)
{
  this->solved = true;
  this->labels.clear();
  this->labels[label] = true;
  for (auto n : this->neighbours)
    n->update(label);
}

void node::update (int label)
{
  if (this->solved) return;

  this->labels.erase(label);

  if (this->check_unique(label, this->row)
      || this->check_unique(label, this->column)
      || this->check_unique(label, this->block))
    return;

  if (this->labels.size() == 1)
    this->set(this->labels.begin()->first);
}

bool node::check_unique (int label, std::vector<std::shared_ptr<node>> group)
{
  std::unordered_map<int, bool> glabels;
  for (auto n : group)
    for (auto p : n->labels) glabels[p.first] = true;

  for (auto l : this->labels)
    if (!glabels[l.first]) {
      this->set(l.first);
      return true;
    }

  return false;
}


std::vector<std::shared_ptr<node>> make_board (int size)
{
  std::vector<std::shared_ptr<node>> board;
  int blksize = (int)std::sqrt(size);

  for (int i = 0; i < (size * size); ++i) {
    int x = i % size;
    int y = i / size;
    auto current = std::make_shared<node>(node());

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
        if (samerow) {
          board[k]->row.push_back(current);
          current->row.push_back(board[k]);
        }
        if (samecol) {
          board[k]->column.push_back(current);
          current->column.push_back(board[k]);
        }
        if (sameblk) {
          board[k]->block.push_back(current);
          current->block.push_back(board[k]);
        }

        board[k]->neighbours.push_back(current);
        current->neighbours.push_back(board[k]);
      }
    }

    board.push_back(current);
  }

  return board;
}


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


void set_board (std::string input, std::vector<std::shared_ptr<node>>& board)
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


std::shared_ptr<node> unsolved_node (std::vector<std::shared_ptr<node>>& board)
{
  for (auto n : board)
    if (!(n->solved)) return n;

  return nullptr;
}


// for debugging
std::ostream& operator<< (std::ostream& out, std::unordered_map<int, bool> map)
{
  out << "[";
  for (auto pair : map) out << pair.first << ",";
  out << "]";

  return out;
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

  set_board(input, board);

  // solve greedily
  // auto u = unsolved_node(board);
  // for (; u != nullptr; u = unsolved_node(board))
  //   u->set(u->labels.begin()->first);

  std::cout << format_board(board);

  return 0;
}
