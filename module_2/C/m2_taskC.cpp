#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <regex>
#include <stack>
#include <utility>

//
//
// ----------- Text Interface --------------------------------------------------

enum class Commands {
  Add,     //(K, V)
  Set,     //(K, V)
  Delete,  //(K)
  Search,  //(K)
  Min,
  Max,
  Extract,
  Print,
  Error
};

struct KeyValuePair {
  KeyValuePair(int key, std::string val="") : Key(key), Value(std::move(val)) {}

  int Key;
  std::string Value;
};

std::pair<Commands, KeyValuePair> ParseCommand(const std::string &str) {
  // 1st group - int, 2nd - non-space chars or empty
  static const auto add_pattern = std::regex(R"(add\s([+-]?\d*)\s([^ ]+?|)$)");
  static const auto set_pattern = std::regex(R"(set\s([+-]?\d*)\s([^ ]+?|)$)");
  static const auto delete_pattern = std::regex(R"(delete\s([+-]?\d*)$)");
  static const auto search_pattern = std::regex(R"(search\s([+-]?\d*)$)");
  try {
    std::smatch matches;
    if (std::regex_match(str, matches, add_pattern)) {
      int key = std::stoi(matches[1]);
      return std::make_pair(Commands::Add, KeyValuePair(key, matches[2]));

    } else if (std::regex_match(str, matches, set_pattern)) {
      int key = std::stoi(matches[1]);
      return std::make_pair(Commands::Set, KeyValuePair(key, matches[2]));

    } else if (std::regex_match(str, matches, delete_pattern)) {
      int key = std::stoi(matches[1]);
      return std::make_pair(Commands::Delete, KeyValuePair(key, ""));

    } else if (std::regex_match(str, matches, search_pattern)) {
      int key = std::stoi(matches[1]);
      return std::make_pair(Commands::Search, KeyValuePair(key, ""));

    } else if (str == "min") {
      return std::make_pair(Commands::Min, KeyValuePair(0, ""));

    } else if (str == "max") {
      return std::make_pair(Commands::Max, KeyValuePair(0, ""));

    } else if (str == "extract") {
      return std::make_pair(Commands::Print, KeyValuePair(0, ""));

    } else if (str == "print") {
      return std::make_pair(Commands::Print, KeyValuePair(0, ""));
    }
  } catch (...) {
  }
  return std::make_pair(Commands::Error, KeyValuePair(0, ""));
}

void InteractWithBinTreeByTextCommands(std::istream &in, std::ostream &out) {
  BinaryTree<int, std::string> tree{};

  std::string line;
  while (std::getline(in, line)) {
    if (line.empty()) continue;
    //  while (in >> line) {
    //    if (line == "\n") continue;
    std::pair<Commands, KeyValuePair> cmd_and_args = ParseCommand(line);

    switch (cmd_and_args.first) {
      case Commands::Add:
        tree.Add(cmd_and_args.second.Key, cmd_and_args.second.Value);
        break;

      case Commands::Set:
        tree.Set(cmd_and_args.second.Key, cmd_and_args.second.Value);
        break;

      case Commands::Delete:
        tree.Delete(cmd_and_args.second.Key);
        break;

      case Commands::Search:
        out << tree.Search(cmd_and_args.second.Key) << std::endl;
        break;

      case Commands::Min:
        out << tree.Min().first << std::endl;
        break;

      case Commands::Max:
        out << tree.Max().first << std::endl;
        break;

      case Commands::Print:
        out << tree << std::endl;
        break;

      case Commands::Error:
        out << "error" << std::endl;
        break;
    }
  }
}

int main() {
  InteractWithBinTreeByTextCommands(std::cin, std::cout);
  //  static const auto add_pattern =
  //      std::regex(R"(add ([+]?\d+) ([^ ]+)(\r\n|\r|\n))");
  //  std::smatch matches;
  //  std::regex_match(, matches, add_pattern);
  //  int key = std::stoi(matches[1]);
  //  BinaryTree<int, std::string> tree{};
  //  tree.Add(5, "root");
  //  tree.Add(2, "s");
  //  tree.Add(3, "s");
  //  tree.Add(7, "Hello from node");
  //  tree.Add(6, "s");
  //  tree.Add(11, "s");
  //  tree.Add(10, "s");
  //  tree.Add(4, "s");
  //  std::cout << tree << std::endl;
  //  tree.Delete(3);
  //  auto val = tree.Search(3);
  //  std::cout << tree << std::endl;

  return 0;
}
