#include <boost/regex.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

struct StackException : std::runtime_error {
  explicit StackException(const std::string &what_arg) : std::runtime_error(what_arg) {}
};

template<typename T>
class Stack {
 public:
  explicit Stack(size_t size) : size_(size), raw_array_(new T[size]()), head_index_(-1) {
  }

  // ---- Main Interface ---------------------

  void push(T &&value) {
    // В случае переполнения стека выводится "overflow".
    if (head_index_ + 1 >= size_) {
      throw StackException("overflow");
    }
    raw_array_[++head_index_] = std::move(value);
  }

  T pop() {
    // Команда pop выводит элемент или "underflow", если стек пуст.
    if (Empty()) {
      throw StackException("underflow");
    }
    T res = raw_array_[head_index_];
    raw_array_[head_index_--] = "";
    return res;
  }

  void print() {
    // Или вынести в отдельную функцию
    // Команда print выводит содержимое стека (снизу вверх) одной строкой, значения разделяются пробелами. Если стек пуст, то выводится "empty".
    if (Empty()) {
      throw StackException("empty");
    }
    for (int i = 0; i < head_index_; ++i) {
      std::cout << raw_array_[i] << ' ';
    }
  }

  // ---- Additional ---------------------
  bool Empty() {
    return head_index_ == -1;
  }

 private:
  size_t size_;
  T *raw_array_;
  int head_index_;
};

int main(int argc, char *argv[]) {
  std::string command;
  while (std::cin >> command) {
    if (command.empty()) continue;
    if (command == "set_size")
      ;
  }
  Stack<std::string> stack(10);
  stack.print();
  return 0;
}
//
//int main() {
//  std::string xStr("AAAA-12222-BBBBB-44455");
//  boost::regex xRegEx("(\\w+)-(\\d+)-(\\w+)-(\\d+)");
//  boost::smatch xResults;
//
//  std::cout << "==========================Results============================== \n";
//  std::cout << "Does this line match our needs? " << std::boolalpha << boost::regex_match(xStr, xResults, xRegEx) << "\n";
//  std::cout << "Print entire match:\n " << xResults[0] << std::endl;
//  std::cout << "Print the former string into another format:\n"
//            << xResults[1] << "+"
//            << xResults[2] << "+"
//            << xResults[3] << "+"
//            << xResults[4] << std::endl;
//}
