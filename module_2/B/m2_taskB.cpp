// Copyright 2021 Fedor Teleshov <fdrt29@gmail.com>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <stack>
#include <utility>

template <class K, class V>
class IBinaryTree {
 public:
  virtual ~IBinaryTree() = default;
  virtual void Add(K, V &&) = 0;
  virtual void Set(K, V &&) = 0;
  virtual void Delete(K) = 0;
  virtual V Search(
      K) const = 0;  // In std::map returns Iterator to an element with key
                     // equivalent to key. If no such element is found,
                     // past-the-end (see end()) iterator is returned.
                     // TODO mb use iterator
  virtual std::pair<K, V> Min() const = 0;
  virtual std::pair<K, V> Max() const = 0;
};
// TODO mb implement iterator

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

    friend std::ostream &operator<<(std::ostream &out, Node node) {
      out << "[" << node.key << " " << node.value << " " << node.parent << "]";
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
  //  TODO при вставке ноды == уже имеющийся нужно выводить error => throw or
  //  bool
  void Add(Key key, Value &&value) override {
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
    // TODO mb i should add node with equal key to one
    // side(e.g. left). Don't rotate when you have equal nodes!
    // Traverse down to the next level and rotate that!
  }

  void Set(Key key, Value &&value) override {
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
        current->parent->*current->PositionInParent() = nullptr;  //:)
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

  void LevelorderTraversal(Node *root, std::function<void(Node *, int level)> callback) {
    std::queue<Node *> queue;
    Node *node = root;
    queue.push(node);
    int level = 0;
    while (not queue.empty()) {
      node = queue.front();
      queue.pop();
      level = asf; //TODO
      callback(node, level);
      if (node->left != nullptr) queue.push(node->left);
      if (node->right != nullptr) queue.push(node->right);
    }
  }

  friend std::ostream &operator<<(std::ostream &out, BinaryTree tree) {
    tree.LevelorderTraversal(tree.root_, [&out](Node *node) { out << node; });
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
        break;  // TODO change if key duplicate are possible
      }
    }
  }  // TODO mb make returned value more rich, mb rename

 private:
  Node *root_;
};

int main() {
  BinaryTree<int, std::string> tree{};
  tree.Add(5, "root");
  tree.Add(2, "");
  tree.Add(0, "");
  tree.Add(3, "");
  tree.Add(7, "Hello from node");
  tree.Add(6, "");
  tree.Add(11, "");
  tree.Add(10, "");
  tree.Add(4, "");
  tree.Delete(3);
  auto val = tree.Search(3);
  std::cout << tree << std::endl;
  return 0;
}
