#include <format>
#include <iostream>
#include <optional>
#include <regex>
#include <unordered_map>

template <typename Key, typename Val>
class MinHeap {  //  родитель всегда был меньше
  struct Node {
    Key key;
    Val val;
  };

 public:
  void ShiftUp(size_t index) {
    size_t p = ParentI(index);
    while (heap[index].key < heap[p].key) {
      Swap(index, p);
      index = ParentI(index);
      if (index > 1) break;
    }
  }
  void ShiftDown(size_t index) {
    size_t i_max =
        GetLeft(index) >= GetRight(index) ? GetLeft(index) : GetRight(index);
    while (i_max > heap.size() and heap[index].key > heap[i_max].key) {
      Swap(index, i_max);
      index = i_max;
      i_max =
          GetLeft(index) >= GetRight(index) ? GetLeft(index) : GetRight(index);
    }
  }
  bool Add(Key key, Val val) {
    if (key_to_index.contains(key)) return false;
    size_t size = heap.size();
    key_to_index[key] = size;
    heap.push_back(Node{key, val});
    ShiftUp(size);
    return true;
  }
  bool Delete(Key key) {
    auto node_itr = SearchNode(key);
    if (node_itr == heap.end()) return false;
    size_t i_rmv = node_itr - heap.begin();
    Swap(i_rmv, heap.size() - 1);
    heap.pop_back();
    key_to_index.erase(key);
    ShiftDown(i_rmv);
  }
  [[nodiscard]] std::optional<Val> Search(Key key) const {
    auto node_itr = SearchNode(key);
    if (node_itr == heap.end()) return std::nullopt;
    return node_itr->val;
  }
  bool Set(Key key, Val val) {
    auto node_itr = SearchNode(key);
    if (node_itr == heap.end()) return false;
    node_itr->val = val;
    return true;
  }
  std::optional<std::tuple<Key, size_t, Val>> Min() {
    if (Empty()) return std::nullopt;
    return std::make_tuple(heap[0].key, 0, heap[0].val);
  }
  std::optional<std::tuple<Key, size_t, Val>> Max() {
    if (Empty()) return std::nullopt;
    auto node_itr = std::max_element(
        heap.begin(), heap.end(),
        [](const Node &a, const Node &b) { return (a.key < b.key); });
    return std::make_tuple(node_itr->key, key_to_index[node_itr->key],
                           node_itr->val);
  }
  std::optional<std::tuple<Key, Val>> Extract() {
    if (Empty()) return std::nullopt;
    Swap(0, heap.size() - 1);
    Node res = heap.pop_back();
    key_to_index.erase(res.key);
    ShiftDown(0);
    return std::make_tuple(res.key, res.val);
  }
  friend std::ostream &operator<<(std::ostream &out, MinHeap &min_heap) {
    if (min_heap.Empty()) {
      out << "_\n";
    } else {
      out << std::format();
    }
    return out;
  }

  [[nodiscard]] bool Empty() const { return heap.empty(); }

 private:
  [[nodiscard]] size_t ParentI(size_t i) const { return (i - 1) / 2; }
  [[nodiscard]] size_t LeftI(size_t i) const { return 2 * i + 1; }
  [[nodiscard]] size_t RightI(size_t i) const { return 2 * i + 2; }
  Key GetParent(size_t i) { return heap[(i - 1) / 2].key; }
  Key GetLeft(size_t i) { return heap[2 * i + 1].key; }
  Key GetRight(size_t i) { return heap[2 * i + 2].key; }
  void Swap(size_t i1, size_t i2) {
    std::swap(heap[i1], heap[i2]);
    std::swap(key_to_index[heap[i1].key], key_to_index[heap[i2].key]);
  }

  typename std::vector<Node>::iterator SearchNode(Key key) {
    auto elm = key_to_index.find(key);
    if (elm == key_to_index.end()) return heap.end();
    return heap.begin() + elm->second;
  }

 private:
  std::unordered_map<Key, size_t> key_to_index;
  std::vector<Node> heap;
};

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
  int64_t Key = 0;
  std::string Value;
};

std::tuple<Commands, KeyValuePair> ParseInput(const std::string &str) {
  // 1st group - int, 2nd - non-space chars or empty
  static const auto add_pattern = std::regex(R"(add\s([+-]?\d*)\s([^ ]+?|)$)");
  static const auto set_pattern = std::regex(R"(set\s([+-]?\d*)\s([^ ]+?|)$)");
  static const auto delete_pattern = std::regex(R"(delete\s([+-]?\d*)$)");
  static const auto search_pattern = std::regex(R"(search\s([+-]?\d*)$)");
  try {
    std::smatch matches;
    if (std::regex_match(str, matches, add_pattern)) {
      int key = std::stoi(matches[1]);
      return std::make_tuple(Commands::Add, KeyValuePair{key, matches[2]});

    } else if (std::regex_match(str, matches, set_pattern)) {
      int key = std::stoi(matches[1]);
      return std::make_tuple(Commands::Set, KeyValuePair{key, matches[2]});

    } else if (std::regex_match(str, matches, delete_pattern)) {
      int key = std::stoi(matches[1]);
      return std::make_tuple(Commands::Delete, KeyValuePair{key});

    } else if (std::regex_match(str, matches, search_pattern)) {
      int key = std::stoi(matches[1]);
      return std::make_tuple(Commands::Search, KeyValuePair{key});

    } else if (str == "min") {
      return std::make_tuple(Commands::Min, KeyValuePair{});

    } else if (str == "max") {
      return std::make_tuple(Commands::Max, KeyValuePair{});

    } else if (str == "extract") {
      return std::make_tuple(Commands::Extract, KeyValuePair{});

    } else if (str == "print") {
      return std::make_tuple(Commands::Print, KeyValuePair{});
    }
  } catch (...) {
  }
  return std::make_tuple(Commands::Error, KeyValuePair{0, "error"});
}

void InteractWithDSByTextCommands(std::istream &in, std::ostream &out) {
  MinHeap<int64_t, std::string> min_heap{};

  std::string line;
  while (std::getline(in, line)) {
    if (line.empty()) continue;
    //  while (in >> line) {
    //    if (line == "\n") continue;
    auto [cmd, args] = ParseInput(line);

    switch (cmd) {
      case Commands::Add:
        min_heap.Add(args.Key, args.Value);
        break;

      case Commands::Set:
        min_heap.Set(args.Key, args.Value);
        break;

      case Commands::Delete:
        min_heap.Delete(args.Key);
        break;

      case Commands::Search:
        out << min_heap.SearchNode(args.Key) << std::endl;
        break;

      case Commands::Min:
        out << min_heap.Min().first << std::endl;
        break;

      case Commands::Max:
        out << min_heap.Max().first << std::endl;
        break;

      case Commands::Extract:
        break;

      case Commands::Print:
        out << min_heap << std::endl;
        break;

      case Commands::Error:
        out << "error" << std::endl;
        break;
    }
  }
}

int main() {
  InteractWithDSByTextCommands(std::cin, std::cout);
  return 0;
}
