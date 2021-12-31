#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>

using namespace std;

wstring lowercase(const wstring &str) {
  thread_local locale locale_{""};
  wstring result;
  for_each(str.begin(), str.end(), [&result](const wchar_t symbol) {
    result.push_back(tolower(symbol, locale_));
  });
  return result;
}

class Trie {
 private:
  struct Node {
    Node(wstring key_, bool end = false)
        : key(key_), right(nullptr), child(nullptr), end(end) {}

    Node *right;
    Node *child;
    wstring key;
    bool end;
  };
  Node *root;

 public:
  Trie() : root(nullptr) {}

  ~Trie() {
    if (root) {
      clear(root);
    }
  }

  set<wstring> search(const wstring &word, unsigned int max_cost = 1) const {
    auto target = lowercase(word);
    vector<unsigned int> current;
    for (unsigned int i = 0; i <= target.size() + 1; ++i) {
      current.push_back(i);
    }
    set<wstring> result;

    wstring current_word;
    vector<unsigned int> prev_row;
    Node *node = root;
    while (node) {
      r_search(node, node->key, target, prev_row, current, result,
               current_word, max_cost);
      node = node->right;
    }
    return result;
  }

  void insert(wstring &word) {
    auto result = lowercase(word);
    root = insert(root, result);
  }

 private:
  Node *insert(Node *node, const wstring &word, unsigned int count = 0) {
    auto target = lowercase(word);
    if (count == 0) {
      count = target.size();
    }
    if (node == nullptr) {
      return new Node(target, true);
    }
    auto k = prefix(target, node->key);
    if (k == 0) {
      node->right = insert(node->right, target, count);
    } else if (k < count) {
      if (k < node->key.size()) {
        split(node, k);
        node->end = false;
      }

      node->child =
          insert(node->child, wstring(word.begin() + k, word.end()), count - k);
    } else if (k == count) {
      if (k < node->key.size()) {
        split(node, k);
      }
      node->end = true;
    }
    return node;
  }

  // Сложность O(n*m), где n - длина слова, m - количество узлов в дереве
  // От корня рекурсивно проходимся по всем правым братьям, проверяя количество
  // ошибок, если их больше 1 выходим из этой ветви алгоритма
  void r_search(const Node *node, const wstring &letters,
                const wstring &word, vector<unsigned int> vector_row,
                vector<unsigned int> previous, set<wstring> &result,
                wstring current_word, unsigned int max_cost) const {
    auto columns = word.size() + 2;
    vector<unsigned int> current;
    for (unsigned int i = 0; i < letters.size(); ++i) {
      current_word.push_back(letters[i]);
      current.clear();
      current.push_back(previous[0] + 1);

      for (unsigned int column = 1; column < columns; ++column) {
        unsigned int insert_cost = current[column - 1] + 1;
        unsigned int delete_cost = previous[column] + 1;

        unsigned int replace_cost;
        if (word[column - 1] != letters[i]) {
          replace_cost = previous[column - 1] + 1;
        } else {
          replace_cost = previous[column - 1];
        }

        current.push_back(min(min(insert_cost, delete_cost), replace_cost));
        if (!vector_row.empty() && column >= 2 &&
            word[column - 2] == letters[i] &&
            word[column - 1] == current_word[current_word.size() - 2]) {
          current[column] =
              min(current[column], vector_row[column - 2] + 1);
        }
      }

      if (current[current.size() - 2] <= max_cost && node->end &&
          i == letters.size() - 1) {
        result.insert(current_word);
      }

      if (*min_element(current.begin(), current.end()) <= max_cost &&
          i != letters.size() - 1) {
        vector_row = previous;
        previous = current;
      } else {
        break;
      }
    }

    if (*min_element(current.begin(), current.end()) <= max_cost) {
      Node *child_node = node->child;
      while (child_node) {
        r_search(child_node, child_node->key, word, previous,
                 current, result, current_word, max_cost);
        child_node = child_node->right;
      }
    }
  }

  unsigned int prefix(const wstring &str, const wstring &key) const {
    for (unsigned int k = 0; k < str.size(); ++k) {
      if (k == key.size() || str[k] != key[k])
        return k;
    }
    return str.size();
  }

  void split(Node *node, unsigned int k) {
    auto *target = new Node(
        std::wstring(node->key.begin() + k, node->key.end()), node->end);
    target->child = node->child;
    node->child = target;
    node->key = wstring(node->key.begin(), node->key.begin() + k);
  }

  void clear(Node *node) {
    if (node->right) {
      clear(node->right);
    }
    if (node->child) {
      clear(node->child);
    }
    delete node;
  }
};

int main() {
  ios_base::sync_with_stdio(false);
  locale locale_{""};
  wcin.imbue(locale_);
  wcout.imbue(locale_);
  Trie tree;
  wstring str;

  auto words_count = 0;
  wcin >> words_count;

  wcin.ignore();

  while (words_count) {
    getline(wcin, str);
    tree.insert(str);
    --words_count;
  }

  while (getline(wcin, str)) {
    if (!str.empty()) {
      wcout << str;
      auto search_str = lowercase(str);
      set<wstring> result_set = tree.search(search_str);
      if (result_set.empty()) {
        wcout << L" -?\n";
      } else {
        if (result_set.count(search_str) != 0) {
          wcout << L" - ok\n";
        } else {
          wstring correct_words = L" ->";
          for (const auto &print : result_set) {
            correct_words += L" " + print + L",";
          }
          correct_words.pop_back();
          wcout << correct_words << L"\n";
        }
      }
    }
  }

  return 0;
}
