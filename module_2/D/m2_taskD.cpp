#include <algorithm>
#include <iostream>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <vector>

typedef int64_t key_type;
typedef std::string val_type;

size_t MatchEndPosition(std::string what, std::string where) {
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
     * максимального совпадающего с меткой префикса вплоть до конца
     * суффикса в худшем случае, в остальных - остаток суффикса необходимо
     * вставить.
     * По памяти: O(1) - не зависит от входа - вставляется 0-2 узла,
     * когда найдено место.
     * */
    Node* traverse_node = root_;

    while (true) {
      auto itr_edge = traverse_node->edges.find(word[0]);
      if (itr_edge == traverse_node->edges.end()) {
        // Traverse node hasn't matching edge
        // Create new edge marked {word}
        traverse_node->edges[word[0]] = Edge{word, new Node(true)};
        break;
      }

      auto& edge = itr_edge->second;
      auto label = edge.label;
      // Traverse node has matching edge.
      // 4 cases. > and < mean substr
      // word == label
      //      mark pointed node as end
      // word > label
      //      word = word[i_end:]; continue
      // word < label
      //      cur edges[word] = new node();
      //      new_node.edges[label[i_end:]] = old_node;
      //      setup old edge from cur edges
      // have equal prefix
      //      prefix = word[:i_end];
      //      cur edges[prefix] = new node();
      //      new_node.edges[label[i_end:]] = old node;
      //      new_node.edges[word[i_end:]] = new node();
      if (word == label) {
        edge.node_on_end->is_word_end = true;
        return;
      }
      // i_end points to next after match end
      auto i_end = MatchEndPosition(word, edge.label);

      if (i_end == label.size() and i_end < word.size()) {
        word = word.substr(i_end);
        traverse_node = edge.node_on_end;
        continue;

      } else if (i_end == word.size() and i_end < label.size()) {
        // Copy old edge
        auto old_edge = traverse_node->edges[label[0]];
        // Create new edge
        auto new_node = new Node(true);
        traverse_node->edges[label[0]] = Edge{word, new_node};
        // Setup old edge
        auto new_label = label.substr(i_end);
        old_edge.label = new_label;
        // move old edge to new node
        new_node->edges[new_label[0]] = std::move(old_edge);
        return;

      } else if (i_end < word.size() and i_end < label.size()) {
        auto prefix = word.substr(0, i_end);
        auto old_edge = traverse_node->edges[label[0]];

        auto new_inner_node = new Node(false);
        traverse_node->edges[label[0]] = Edge{prefix, new_inner_node};
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

  std::set<std::string> Similar(const std::string& word,
                                uint max_mistake_count = 1) {
    std::set<std::string> results;

    typedef std::tuple<int, Node*, std::string> el_type;
    // min куча
    auto cmp = [](const el_type& left, const el_type& right) {
      return std::get<0>(left) > std::get<0>(right);
    };
    // Так как функция ищет и точное совпадение, то это ускоряет его
    // поиск, взамен stack\queue. Кроме того было просто навязчивое желание
    // использовать, изученное в курсе АиСД.
    std::priority_queue<el_type, std::vector<el_type>, decltype(cmp)>
        priority_queue(cmp);

    priority_queue.emplace(0, root_, "");

    while (not priority_queue.empty()) {
      auto [_, traverse_node, path] = priority_queue.top();
      priority_queue.pop();

      for (auto& [ch, edge] : traverse_node->edges) {
        auto row =
            DamerauLevenshtein(path + edge.label, word, max_mistake_count);

        if (row.back() == 0) {
          return {path + edge.label};
        }
        if (row.back() == max_mistake_count and edge.node_on_end->is_word_end) {
          results.insert(path + edge.label);
        }
        // If size are not equal, returned row is not last in the table.
        // The reason is optimization inside the DamerauLevenshtein() function.
        if (row[0] == path.length() + edge.label.length()) {
          // row.back as priority
          priority_queue.emplace(row.back(), edge.node_on_end,
                                 path + edge.label);
        }
      }
    }

    return results;
  }

 private:
  // Returns last row of Damerau-Levenshtein table or last row that have value
  // <=1.
  static std::vector<uint> DamerauLevenshtein(const std::string& str1,
                                              const std::string& str2,
                                              uint max_mistake_count = 1) {
    auto n = str1.length() + 1;
    auto m = str2.length() + 1;

    std::vector<uint> pre_previous(m);

    std::vector<uint> previous(m);
    std::iota(previous.begin(), previous.end(), 0);

    for (auto i = 1; i < n; i++) {
      std::vector<uint> current(m);
      current[0] = i;
      bool curr_has_valid_mistakes_count = false;
      for (auto j = 1; j < m; j++) {
        uint cost = str1[i - 1] == str2[j - 1] ? 0 : 1;

        uint insert = previous[j] + 1;
        uint del = current[j - 1] + 1;
        uint replace = previous[j - 1] + cost;

        current[j] = std::min({insert, del, replace});

        if (i > 1 && j > 1 && str1[i - 1] == str2[j - 2] &&
            str1[i - 2] == str2[j - 1]) {
          // transposition
          current[j] = std::min(current[j], pre_previous[j - 2] + cost);
        }
        if (not curr_has_valid_mistakes_count and
            current[j] <= max_mistake_count)
          curr_has_valid_mistakes_count = true;
      }
      pre_previous = previous;
      previous = current;
      // Optimisation.
      if (not curr_has_valid_mistakes_count) return previous;
    }

    return previous;
  }

 private:
  Node* root_ = new Node(false);
};

//
//
// ----------- Text Interface --------------------------------------------------

void InteractWithTextCommands(std::istream& in, std::ostream& out) {
  CompressedTrie trie{};
  std::string line;

  key_t n = 0;
  std::cin >> n;
  std::cin.ignore();
  for (int i = 0; i < n; ++i) {
    std::getline(std::cin, line);
    std::transform(line.begin(), line.end(), line.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    trie.Insert(line);
  }

  while (std::getline(in, line)) {
    if (line.empty()) continue;

    std::transform(line.begin(), line.end(), line.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    std::set<std::string> res = trie.Similar(line);
    if (res.empty()) {
      std::cout << line << " - ?" << std::endl;
      continue;
    }
    if (res.find(line) != res.end()) {
      std::cout << line << " - ok" << std::endl;
    } else {
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
  InteractWithTextCommands(std::cin, std::cout);
  return 0;
}
