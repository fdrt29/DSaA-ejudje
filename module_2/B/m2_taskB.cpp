// Copyright 2021 Fedor Teleshov <fdrt29@gmail.com>
#include <functional>
#include <memory>
#include <utility>

template <class K, class V>
class IBinaryTree {
 public:
  virtual void Add(K, V &&) = 0;
  virtual void Set(K, V &&) = 0;
  virtual void Delete(K) = 0;
  virtual V Search(K) const = 0;
  virtual std::pair<K, V> Min() const = 0;
  virtual std::pair<K, V> Max() const = 0;
  virtual void Print(const std::ostream &) const = 0;
};

enum class Commands {
  Add,     //(K, V)
  Set,     //(K, V)
  Delete,  //(K)
  Search,  //(K)
  Min,
  Max,
  Print
};

template <class Key, class Value>
class BinaryTree : IBinaryTree<Key, Value> {
  struct Node {
    Node(Key key_p, Value value_p) noexcept
        : key(key_p), value(std::move(value_p)) {}
    Key key;
    Value value;
    Node *parent;
    Node *left;
    Node *right;
  };

 public:
  BinaryTree() : root_() {}

 public:
  void Add(Key key, Value &&value) override {
    if (not root_) {
      root_ = new Node(key, value);
      return;
    }

    SearchNode(
        key,
        [&key, &value](Node *current) {
          if (current->left == nullptr) current->left = new Node(key, value);
        },
        [&key, &value](Node *current) {
          if (current->right == nullptr) current->right = new Node(key, value);
        },
        [&value](Node *current) { current->value = value; });
    // TODO mb i should add node with equal key to one
    // side(e.g. left). Don't rotate when you have equal nodes!
    // Traverse down to the next level and rotate that!
  }

  void Set(Key key, Value &&value) override {}

  void Delete(Key k) override {}

  Value Search(Key k) const override { return nullptr; }
  std::pair<Key, Value> Min() const override { return std::pair<Key, Value>(); }
  std::pair<Key, Value> Max() const override { return std::pair<Key, Value>(); }
  void Print(const std::ostream &ostream) const override {}

 private:
  void SearchNode(Key key, std::function<void(Node *)> left_callback,
                  std::function<void(Node *)> right_callback,
                  std::function<void(Node *)> equal_callback) const {
    Node *current = root_;
    while (current != nullptr) {
      if (key < current->key) {
        left_callback(current);
        current = current->left;
      } else if (key > current->key) {
        right_callback(current);
        current = current->right;
      } else if (key == current->key) {
        equal_callback(current);
        break;  // TODO change if key duplicate are possible
      }
    }
  }  // TODO mb make returned value more rich, mb rename

 private:
  Node *root_;
};

int main() {
  BinaryTree<int, std::string> tree{};
  tree.Add(10, "root");
  tree.Add(7, "Hello from node");
  return 0;
}
