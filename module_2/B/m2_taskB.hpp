// Copyright 2021 Fedor Teleshov <fdrt29@gmail.com>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <regex>
#include <stack>
#include <utility>

template <class K, class V>
class IBinaryTree {
 public:
  virtual ~IBinaryTree() = default;
  virtual void Add(K, const V &) = 0;
  virtual void Set(K, const V &) = 0;
  virtual void Delete(K) = 0;
  virtual V Search(
      K) const = 0;  // In std::map returns Iterator to an element with Key
                     // equivalent to Key. If no such element is found,
                     // past-the-end (see end()) iterator is returned.
                     // TODO mb use iterator
  virtual std::pair<K, V> Min() const = 0;
  virtual std::pair<K, V> Max() const = 0;
};
// TODO mb implement iterator

template <class Key, class Value>
class BinaryTree : IBinaryTree<Key, Value> {
  struct Node {
    Key key;
    Value value;
    Node *parent;
    Node *left;
    Node *right;

    Node(Key key_p, Value value_p, Node *parent_p) noexcept
        : key(key_p),
          value(std::move(value_p)),
          parent(parent_p),
          left(nullptr),
          right(nullptr) {}

    ~Node() {
      delete left;
      delete right;
    }

    void Clear() {
      left = nullptr;
      right = nullptr;
      parent = nullptr;
    }

    static Node *Max(Node *root) {
      Node *current = root;
      while (true) {
        if (current->right == nullptr) {
          return current;
        }
        current = current->right;
      }
    }

    friend std::ostream &operator<<(std::ostream &out, const Node &node) {
      out << "[" << node.key << " " << node.value;
      if (node.parent) out << " " << node.parent->key;
      out << "]";
      return out;
    }

    [[nodiscard]] Node *Node::*PositionInParent() const {
      if (not parent) return nullptr;
      if (parent->left == this)
        return &Node::left;
      else
        return &Node::right;
    }
  };

 public:
  BinaryTree() : root_() {}
  ~BinaryTree() override { delete root_; }

 public:
  //  TODO ? при вставке ноды с уже имеющимся ключом нужно выводить error =>
  //  throw or bool. Вообще в тестах ошибка была при set вроде
  void Add(Key key, const Value &value) override {
    if (not root_) {
      root_ = new Node(key, value, nullptr);
      return;
    }
    SearchPlaceForKey(
        key,
        [&key, &value](Node *current) {
          if (current->left == nullptr)
            current->left = new Node(key, value, current);
        },
        [&key, &value](Node *current) {
          if (current->right == nullptr)
            current->right = new Node(key, value, current);
        },
        [&value](Node *current) { current->value = value; });
    // TODO mb i should add node with equal Key to one
    // side(e.g. left). Don't rotate when you have equal nodes!
    // Traverse down to the next level and rotate that!
  }

  void Set(Key key, const Value &value) override {
    SearchPlaceForKey(key, nullptr, nullptr,
                      [&value](Node *current) { current->value = value; });
  }

  void Delete(Key key) override {
    if (root_ == nullptr) return;
    SearchPlaceForKey(key, nullptr, nullptr, [](Node *current) {
      if (current->left and current->right) {  // if node haven both children
        // Swap current with max in left subtree
        Node *maxNode = Node::Max(current->left);
        std::swap(maxNode->key, current->key);
        // MaxNode in the left subtree can have a child (only left)
        // Replace ptr in maxNode's parent
        // Using pointer-to-member, just for practice
        maxNode->parent->*maxNode->PositionInParent() = maxNode->left;
        maxNode->Clear();
        delete maxNode;

      } else if (current->left) {
        current->left->parent = current->parent;
        current->parent->*current->PositionInParent() = current->left;
        current->Clear();
        delete current;

      } else if (current->right) {
        current->right->parent = current->parent;
        current->parent->*current->PositionInParent() = current->right;
        current->Clear();
        delete current;

      } else {  // if node haven't children
        current->parent->*current->PositionInParent() = nullptr;
        current->Clear();
        delete current;
      }
    });
  }

  [[nodiscard]] Value Search(Key key) const override {
    SearchPlaceForKey(key, nullptr, nullptr,
                      [](Node *current) { return current->value; });
    return Value();
  }

  // TODO mb use KeyValuePair
  [[nodiscard]] std::pair<Key, Value> Min() const override {
    if (root_ == nullptr) std::pair<Key, Value>();
    Node *current = root_;
    while (true) {
      if (current->left == nullptr) {
        return std::make_pair(current->key, current->value);
      }
      current = current->left;
    }
  }
  // TODO clarify the interaction between the Node::Max function and this
  [[nodiscard]] std::pair<Key, Value> Max() const override {
    if (root_ == nullptr) std::pair<Key, Value>();
    Node *current = root_;
    while (true) {
      if (current->right == nullptr) {
        return std::make_pair(current->key, current->value);
      }
      current = current->right;
    }
  }

  void LevelOrderTraversal(
      Node *root, std::function<void(const Node *, int, bool)> callback) const {
    if (not root_) return;
    std::queue<Node *> queue;
    Node *node = root;
    queue.push(node);
    size_t level = 0;
    bool is_next_level = true;
    while (not queue.empty()) {
      size_t n = queue.size();  // There is only one whole level in queue now
      for (size_t i = 0; i < n; ++i) {
        node = queue.front();
        queue.pop();
        callback(node, level, is_next_level);
        if (node) {
          queue.push(node->left);  // Also if == nullptr
          queue.push(node->right);
        }
        is_next_level = false;
      }
      level++;
      is_next_level = true;
    }
  }

  [[nodiscard]] bool Empty() const { return root_ == nullptr; }

 public:
  friend std::ostream &operator<<(std::ostream &out, const BinaryTree &tree) {
    if (tree.Empty()) out << "_";
    tree.LevelOrderTraversal(  //
        tree.root_,            //
        [&out](const Node *node, [[maybe_unused]] size_t level,
               bool is_next_level) {
          if (level != 0) {
            if (is_next_level)
              out << std::endl;
            else
              out << " ";
          }
          if (node)
            out << *node;
          else
            out << "_";
        });
    return out;
  }

 private:
  void SearchPlaceForKey(Key key, std::function<void(Node *)> left_callback,
                         std::function<void(Node *)> right_callback,
                         std::function<void(Node *)> equal_callback) const {
    Node *current = root_;
    while (current != nullptr) {
      if (key < current->key) {
        if (left_callback) left_callback(current);
        current = current->left;
      } else if (key > current->key) {
        if (right_callback) right_callback(current);
        current = current->right;
      } else if (key == current->key) {
        if (equal_callback) equal_callback(current);
        break;  // TODO change if Key duplicate are possible
      }
    }
  }  // TODO mb make returned Value more rich, mb rename

 private:
  Node *root_;
};

enum class Commands {
  Add,     //(K, V)
  Set,     //(K, V)
  Delete,  //(K)
  Search,  //(K)
  Min,
  Max,
  Print,
  Error
};

struct KeyValuePair {
  KeyValuePair(int key, std::string val) : Key(key), Value(std::move(val)) {}

  int Key;
  std::string Value;
};

std::pair<Commands, KeyValuePair> ParseCommand(const std::string &str) {
  static const auto add_pattern = std::regex(R"(add ([+]?\d+) ([^ ]+)$)");
  static const auto set_pattern =
      std::regex(R"(set ([+]?\d+) ([^ ]+)$)");
  static const auto delete_pattern =
      std::regex(R"(delete ([+]?\d+)$)");
  static const auto search_pattern =
      std::regex(R"(search ([+]?\d+) ([^ ]+)$)");
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
