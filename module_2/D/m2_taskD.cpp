#include <algorithm>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>

typedef int64_t key_type;
typedef std::string val_type;

size_t match_end_position(std::string what, std::string where) {
  auto itr_what = what.begin();
  auto itr_where = where.begin();
  size_t res = 0;
  while (itr_what != what.end() and itr_where != where.end() and
         *itr_what == *itr_where) {
    res++;
    itr_what++;
    itr_where++;
  }
  return res++;  // res points to next after match end
}

class CompressedTrie {
  struct Node;
  struct Edge {
    std::string label;
    Node* node_on_end;
  };
  struct Node {
    explicit Node(bool word_end) : is_word_end(word_end), edges() {}
    bool is_word_end = false;
    // Store as key first char, whole label stored in value with ptr
    std::unordered_map<char, Edge> edges;

    ~Node() {
      for (auto const& el : edges) {
        delete el.second.node_on_end;
      }
    }
  };

 public:
  ~CompressedTrie() { delete root_; }

 public:
  void Insert(std::string word) {
    /* Вставка. Сложность по времени О(|word|), |word|- длина слова.
     * Соответствие первых букв слова и метке ребра находится за
     * константу(хэш-таблица). При каждом посещении вершины необходимо
     * итерироваться по суффиксу изначального слова для определения
     * максимального совпадающего с меткой префикса вплоть до "вырождения"
     * суффикса в худшем случае, в остальных - его необходимо вставить.
     * По памяти: O(1) - не зависит от входа - вставляется 1-2 узла, когда
     * найдено место.
     * */
    Node* cur = root_;

    while (true) {
      auto itr_edge = cur->edges.find(word[0]);
      if (itr_edge == cur->edges.end()) {
        // cur hasn't matching edge
        // Create new edge marked {word}
        cur->edges[word[0]] = Edge{word, new Node(true)};
        break;
      }
      auto& edge = itr_edge->second;
      auto label = edge.label;
      if (word == itr_edge->second.label) {
        edge.node_on_end->is_word_end = true;
        return;
      }
      // i_end points to next after match end
      auto i_end = match_end_position(word, edge.label);
      // >,< mean substr
      // word > label
      //      word = word[i_end:]; continue
      // word < label
      //      cur.edges[word] = new node();
      //      new_node.edges[label[i_end:]] = old_node;
      //      change old edge from cur.edges
      // have equal prefix
      //      prefix = word[:i_end];
      //      cur.edges[prefix] = new node();
      //      new_node.edges[label[i_end:]] = old node;
      //      new_node.edges[word[i_end:]] = new node();
      // word == label
      //      mark pointed node as end
      if (i_end == label.size() and i_end < word.size()) {
        word = word.substr(i_end);
        cur = edge.node_on_end;
        continue;

      } else if (i_end == word.size() and i_end < label.size()) {
        // Copy old edge
        auto old_edge = cur->edges[label[0]];
        // Create new edge
        auto new_node = new Node(true);
        cur->edges[word[0]] = Edge{word, new_node};
        // Setup old edge
        auto new_label = label.substr(i_end);
        old_edge.label = new_label;
        // move old edge to new node
        new_node->edges[new_label[0]] = std::move(old_edge);
        return;
      } else if (i_end < word.size() and i_end < label.size()) {
        auto prefix = word.substr(0, i_end);
        auto old_edge = cur->edges[label[0]];

        auto new_inner_node = new Node(false);
        cur->edges[prefix[0]] = Edge{prefix, new_inner_node};
        // Move old edge to inner node
        auto new_label = label.substr(i_end);
        old_edge.label = new_label;
        new_inner_node->edges[label[i_end]] = std::move(old_edge);
        // Create new edge from inner node to new node
        new_inner_node->edges[word[i_end]] =
            Edge{word.substr(i_end), new Node(true)};
        return;
      }
    }
  }

  bool Search(std::string word) {
    Node* cur = root_;

    while (not word.empty()) {
      auto itr_edge = cur->edges.find(word[0]);
      if (itr_edge == cur->edges.end()) {
        return false;
      }
      if (itr_edge->second.label == word) {
        return itr_edge->second.node_on_end->is_word_end;
      }
      auto i_end = match_end_position(word, itr_edge->second.label);
      word = word.substr(i_end);
      cur = cur->edges[itr_edge->first].node_on_end;
    }
    return false;
  }

  std::vector<std::string> Similar(std::string& word) {
    std::vector<int> current_row(word.size() + 1);
    std::iota(current_row.begin(), current_row.end(), 0);
    std::vector<int> previous_row(word.size() + 1);

    std::vector<std::string> results;
    for (auto& [ch, edge] : root_->edges) {
      std::string prefix = std::to_string(ch);
      SimilarRecursive(edge.node_on_end, prefix, ch, ' ', word,
                       current_row, previous_row, results);
    }
    return results;
  }

 private:
  void SimilarRecursive(Node* node, std::string& prefix, char ch, char prev_ch,
                        std::string& word, std::vector<int>& previous_row,
                        std::vector<int>& pre_previous_row,
                        std::vector<std::string>& results) {
    size_t columns = word.size() + 1;
    std::vector<int> current_row = {previous_row[0] + 1};
    for (int column = 0; column < columns; ++column) {
      int insert_cost = previous_row[column] + 1;
      int delete_cost = current_row[column - 1] + 1;
      int replace_cost = previous_row[column - 1];

      if (word[column - 1] != ch) replace_cost += 1;
      current_row.push_back(
          std::min(std::min(insert_cost, delete_cost), replace_cost));
      if (prev_ch != ' ' and column > 1 and ch == word[column - 2] and
          prev_ch == word[column - 1] and word[column - 1] != ch) {
        current_row[column] =
            std::min(current_row[column], pre_previous_row[column - 2] + 1);
      }
    }
    if (current_row[-1] == 1 and node->is_word_end) {
      results.push_back(prefix);
    }
    if (*std::min_element(current_row.begin(), current_row.end()) <= 1) {
      prev_ch = ch;  // TODO mb отдельная переменная

      for (auto& [sym, child] : node->edges) {
        std::string new_pref = prefix + sym;
        SimilarRecursive(child.node_on_end, new_pref, sym, prev_ch, word,
                         current_row, previous_row, results);
      }
    }
  }

 private:
  Node* root_ = new Node(false);
};

//
//
// ----------- Text Interface --------------------------------------------------

void InteractWithDSByTextCommands(std::istream& in, std::ostream& out) {
  CompressedTrie trie{};
  std::string line;

  key_t n = 0;
  std::cin >> n;
  for (int i = 0; i < n; ++i) {
    std::getline(std::cin, line);
    trie.Insert(line);
  }

  while (std::getline(in, line)) {
    if (line.empty()) continue;

    std::transform(line.begin(), line.end(), line.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    std::vector<std::string> res = trie.Similar(line);
    if (res.empty()) {
      std::cout << line << " -?" << std::endl;
      continue;
    }
    if (std::find(res.begin(), res.end(), line) != res.end()) {
      std::cout << line << " -ok" << std::endl;
    } else {
      std::sort(res.begin(), res.end());
      std::cout << line << " -> ";
      auto i = 0;
      for (const auto& w : res) {
        if (i != 0) std::cout << ", ";
        std::cout << w;
        i++;
      }
      std::cout << std::endl;
    }
  }
}

int main() {
  //  CompressedTrie trie;
  //  trie.Insert("thereafter");
  //  trie.Insert("therein");
  //  trie.Insert("this");
  //  trie.Search("therei");
  InteractWithDSByTextCommands(std::cin, std::cout);
  return 0;
}
