#include <cmath>
#include <iostream>
#include <optional>
#include <regex>
#include <unordered_map>
#include <utility>

typedef int64_t key_type;
typedef std::string val_type;

class MinHeap {  //  родитель всегда был меньше
  struct Node {
    Node(key_type key, val_type val) : key(key), val(std::move(val)) {}
    key_type key;
    val_type val;
  };

 public:
  void ShiftUp(size_t index) {
    while (index >= 1 and heap[index].key < heap[ParentI(index)].key) {
      Swap(index, ParentI(index));
      index = ParentI(index);
    }
  }
  void ShiftDown(size_t index) {
    size_t i_min;
    if (RightI(index) < heap.size() and GetRight(index) < GetLeft(index))
      i_min = RightI(index);
    else
      i_min = LeftI(index);
    while (i_min < heap.size() and heap[index].key > heap[i_min].key) {
      Swap(index, i_min);
      index = i_min;
      if (RightI(index) < heap.size() and GetRight(index) < GetLeft(index))
        i_min = RightI(index);
      else
        i_min = LeftI(index);
    }
  }
  bool Add(key_type key, const val_type &val) {
    auto elm = key_to_index.find(key);
    if (elm != key_to_index.end()) return false;
    size_t size = heap.size();
    key_to_index[key] = size;
    heap.emplace_back(key, val);
    ShiftUp(size);
    return true;
  }
  bool Delete(key_type key) {
    auto node_itr = SearchNode(key);
    if (node_itr == heap.end()) return false;
    size_t i_rmv = node_itr - heap.begin();
    Swap(i_rmv, heap.size() - 1);
    heap.pop_back();
    key_to_index.erase(key);
    if (i_rmv < heap.size()) {
      ShiftDown(i_rmv);
      ShiftUp(i_rmv);
    }
    return true;
  }
  std::optional<std::tuple<size_t, val_type>> Search(key_type key) {
    auto node_itr = SearchNode(key);
    if (node_itr == heap.end()) return std::nullopt;
    return std::make_tuple(node_itr - heap.begin(), node_itr->val);
  }
  bool Set(key_type key, val_type val) {
    auto node_itr = SearchNode(key);
    if (node_itr == heap.end()) return false;
    node_itr->val = std::move(val);
    return true;
  }
  std::optional<std::tuple<key_type, size_t, val_type>> Min() {
    if (Empty()) return std::nullopt;
    return std::make_tuple(heap[0].key, 0, heap[0].val);
  }
  std::optional<std::tuple<key_type, size_t, val_type>> Max() {
    if (Empty()) return std::nullopt;
    auto node_itr = std::max_element(
        heap.begin(), heap.end(),
        [](const Node &a, const Node &b) { return (a.key < b.key); });
    return std::make_tuple(node_itr->key, key_to_index[node_itr->key],
                           node_itr->val);
  }
  std::optional<std::tuple<key_type, val_type>> Extract() {
    if (Empty()) return std::nullopt;
    Swap(0, heap.size() - 1);
    Node res = heap.back();
    heap.pop_back();
    key_to_index.erase(res.key);
    ShiftDown(0);
    return std::make_tuple(res.key, res.val);
  }

  [[nodiscard]] bool Empty() const { return heap.empty(); }

 private:
  static size_t ParentI(size_t i) { return (i - 1) / 2; }
  static size_t LeftI(size_t i) { return 2 * i + 1; }
  static size_t RightI(size_t i) { return 2 * i + 2; }
  key_type GetParent(size_t i) { return heap[(i - 1) / 2].key; }
  key_type GetLeft(size_t i) { return heap[2 * i + 1].key; }
  key_type GetRight(size_t i) { return heap[2 * i + 2].key; }
  void Swap(size_t i1, size_t i2) {
    std::swap(heap[i1], heap[i2]);
    std::swap(key_to_index[heap[i1].key], key_to_index[heap[i2].key]);
  }

  typename std::vector<Node>::iterator SearchNode(key_type key) {
    auto elm = key_to_index.find(key);
    if (elm == key_to_index.end()) return heap.end();
    return heap.begin() + elm->second;
  }

 public:
  friend std::ostream &operator<<(std::ostream &out, const MinHeap &min_heap) {
    if (min_heap.Empty()) {
      out << "_";
      return out;
    }
    out << "[" << min_heap.heap[0].key  //
        << " " << min_heap.heap[0].val  //
        << "]";                         //

    auto levels =
        static_cast<size_t>(std::log(min_heap.heap.size()) / std::log(2) + 1);
    if (levels == 1) return out;
    out << std::endl;

    size_t left = 1;
    size_t right = 2;
    size_t pow2_el_on_lvl = 1;
    for (size_t lvl = 1; lvl < levels; ++lvl) {
      // Уровень находится в памяти непрерывно, поэтому можно найти крайний
      // левый и крайний правый индексы и напечатать подряд
      pow2_el_on_lvl *= 2;
      right = left + pow2_el_on_lvl - 1;

      for (size_t i = left; i <= right; ++i) {
        if (i >= min_heap.heap.size()) {
          out << "_";
        } else {
          out << "[" << min_heap.heap[i].key           //
              << " " << min_heap.heap[i].val           //
              << " " << min_heap.heap[ParentI(i)].key  //
              << "]";
        }
        if (i != right) out << " ";
      }
      if (lvl != levels - 1) out << std::endl;

      left = LeftI(left);
    }

    return out;
  }

 private:
  std::unordered_map<key_type, size_t> key_to_index;
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
  key_type Key = 0;
  val_type Value;
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
      key_type key = std::stoll(matches[1]);
      return std::make_tuple(Commands::Add, KeyValuePair{key, matches[2]});

    } else if (std::regex_match(str, matches, set_pattern)) {
      key_type key = std::stoll(matches[1]);
      return std::make_tuple(Commands::Set, KeyValuePair{key, matches[2]});

    } else if (std::regex_match(str, matches, delete_pattern)) {
      key_type key = std::stoll(matches[1]);
      return std::make_tuple(Commands::Delete, KeyValuePair{key});

    } else if (std::regex_match(str, matches, search_pattern)) {
      key_type key = std::stoll(matches[1]);
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

void error(std::ostream &out) { out << "error" << std::endl; }

void InteractWithDSByTextCommands(std::istream &in, std::ostream &out) {
  MinHeap min_heap{};

  std::string line;
  while (std::getline(in, line)) {
    if (line.empty()) continue;
    auto [cmd, args] = ParseInput(line);

    switch (cmd) {
      case Commands::Add:
        if (not min_heap.Add(args.Key, args.Value)) error(out);
        break;

      case Commands::Set:
        if (not min_heap.Set(args.Key, args.Value)) error(out);
        break;

      case Commands::Delete:
        if (not min_heap.Delete(args.Key)) error(out);
        break;

      case Commands::Search: {
        auto sch = min_heap.Search(args.Key);
        if (sch) {
          auto [i, val] = sch.value();
          out << "1 " << i << " " << val << std::endl;
        } else {
          out << "0" << std::endl;
        }
        break;
      }

      case Commands::Min: {  //"K I V"
        auto min = min_heap.Min();
        if (min) {
          auto [key, i, val] = min.value();
          out << key << " " << i << " " << val << std::endl;
        } else
          error(out);
        break;
      }

      case Commands::Max: {
        auto max = min_heap.Max();
        if (max) {
          auto [key, i, val] = max.value();
          out << key << " " << i << " " << val << std::endl;
        } else
          error(out);
        break;
      }

      case Commands::Extract: {  //"K V"
        auto root = min_heap.Extract();
        if (root) {
          auto [key, val] = root.value();
          out << key << " " << val << std::endl;
        } else
          error(out);
        break;
      }

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
