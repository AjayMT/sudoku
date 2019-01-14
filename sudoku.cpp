
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

  bool set (int label);
  bool update (int label);
  bool check_unique (int label, std::vector<std::shared_ptr<node>> group);
};

bool node::set (int label)
{
  this->solved = true;
  this->labels.clear();
  this->labels[label] = true;

  bool success = true;
  for (auto n : this->neighbours)
    if (!(n->update(label))) success = false;

  return success;
}

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

bool node::check_unique (int label, std::vector<std::shared_ptr<node>> group)
{
  std::unordered_map<int, bool> glabels;
  for (auto n : group)
    for (auto p : n->labels) glabels[p.first] = true;

  for (auto l : this->labels)
    if (!glabels[l.first])
      return this->set(l.first);

  return true;
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


void restore_board (std::vector<std::shared_ptr<node>>& board,
                    std::vector<std::pair<std::unordered_map<int, bool>, bool>>& captured)
{
  for (int i = 0; i < board.size(); ++i) {
    board[i]->labels = captured[i].first;
    board[i]->solved = captured[i].second;
  }
}


bool bruteforce_board (std::vector<std::shared_ptr<node>>& board, std::shared_ptr<node> u)
{
  if (u == nullptr) return true;

  auto labels = u->labels;
  for (auto l : labels) {
    auto captured = capture_board(board);
    bool success = u->set(l.first);
    if (success) return bruteforce_board(board, unsolved_node(board));
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

  set_board(input, board);

  // brute force :(
  bruteforce_board(board, unsolved_node(board));

  std::cout << format_board(board);

  return 0;
}
