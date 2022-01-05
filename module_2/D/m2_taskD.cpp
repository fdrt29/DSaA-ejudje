#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>
#include <set>
#include <stack>
#include <unordered_map>
#include <vector>

size_t MatchEndPosition(std::wstring what, std::wstring where) {
  auto itr_what = what.begin();
  auto itr_where = where.begin();
  size_t res = 0;
  while (itr_what != what.end() and itr_where != where.end() and
         *itr_what == *itr_where) {
    res++;
    itr_what++;
    itr_where++;
  }
  return res;  // res points to next after match end
}

class RadixTrie {
  struct Node;
  struct Edge {
    std::wstring label;
    Node* node_on_end;
  };
  struct Node {
    explicit Node(bool word_end) : is_word_end(word_end), edges() {}
    bool is_word_end = false;
    // Store as key first char, whole label stored in value with ptr
    std::unordered_map<wchar_t, Edge> edges;

    ~Node() {
      for (auto const& el : edges) {
        delete el.second.node_on_end;
      }
    }
  };

 public:
  ~RadixTrie() { delete root_; }

 public:
  /* Вставка.
   * Сложность по времени О(|word|), |word|- длина слова.
   * Соответствие первых букв слова и метке ребра находится за
   * константу(хэш-таблица). При каждом посещении вершины необходимо
   * итерироваться по суффиксу изначального слова для определения
   * максимального совпадающего с меткой префикса вплоть до конца
   * суффикса в худшем случае, в остальных - остаток суффикса необходимо
   * вставить.
   * По памяти: O(1) - не зависит от входа. Вставляется 0-2 узла,
   * когда найдено место.
   * */
  void Insert(std::wstring word) {
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

  /* Нечеткий поиск.
   * Сложность по времени: О(k*n), k - длина искомого слова, n - число узлов в
   * Radix Trie. Для каждого узла, имеющего расстояние 1 хотя бы между 2
   * префиксами искомого слова и метки, проверяем всех его детей. В худшем
   * случае придется пройти по всем узлам: каждый узел будет содержать только
   * одну букву искомого слова и иметь ребенка, удовлетворяющего названному выше
   * условию. Для каждого узла будет рассчитано расстояние Дамерау-Левенштейна
   * за О(k*С). С - константа.
   *
   * Сложность по памяти: О(n+k). О(k) для Дамерау-Левенштейна + память на стек
   * O(n), в который поместятся все узлы в худшем случае: корень имеет n ребер,
   * каждое из которых подходит.
   * */
  /// @return Exact match or otherwise similar words
  std::set<std::wstring> FuzzySearch(const std::wstring& word,
                                     uint max_mistake_count = 1) {
    std::set<std::wstring> results;

    // TODO использовать очередь с приоритетом? log вставка в нее или итерация?
    std::stack<std::pair<Node*, std::wstring>> stack;
    stack.push(std::make_pair(root_, L""));

    while (not stack.empty()) {
      auto [traverse_node, path] = stack.top();
      stack.pop();
      // Traverse node have valid distance, check its children
      for (auto& [ch, edge] : traverse_node->edges) {
        auto current_path = path + edge.label;
        auto o_dist =
            TryDamerauLevenshtein(current_path, word, max_mistake_count);

        // If not true, returned row is not last in the table.
        // The reason is optimization inside the DamerauLevenshtein() function.
        if (o_dist.has_value()) {
          if (o_dist.value() == 0 and edge.node_on_end->is_word_end) {
            return {current_path};
          }
          if (o_dist.value() == max_mistake_count and
              edge.node_on_end->is_word_end) {
            results.insert(current_path);
          }

          stack.push(std::make_pair(edge.node_on_end, current_path));
        }
      }
    }

    return results;
  }

 private:
  /* Расстояние Дамерау-Левенштейна.
   * Сложность по времени: O(k*l). k на инициализацию, k*l итераций по таблице
   * в худшем случае. k - длина искомого слова, l - длина метки ребра
   *
   * Сложность по памяти: О(3(k+1))=O(k). Храним 3 строки
   * таблицы, длина которых ~ длине k искомого слова.
   * */
  static std::optional<uint> TryDamerauLevenshtein(const std::wstring& str1,
                                                   const std::wstring& str2,
                                                   uint max_mistake_count = 1) {
    auto n = str1.length() + 1;
    auto m = str2.length() + 1;

    std::vector<uint> pre_previous(m);

    std::vector<uint> previous(m);
    std::iota(previous.begin(), previous.end(), 0);

    bool prev_has_valid_mistakes_count = false;
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
          // Transposition
          current[j] = std::min(current[j], pre_previous[j - 2] + cost);
        }
        if (not curr_has_valid_mistakes_count and
            current[j] <= max_mistake_count)
          curr_has_valid_mistakes_count = true;
      }
      // Optimisation.
      if (not curr_has_valid_mistakes_count and prev_has_valid_mistakes_count)
        return std::nullopt;  // Early termination of algorithm

      pre_previous = previous;
      previous = current;
      prev_has_valid_mistakes_count = curr_has_valid_mistakes_count;
    }

    return previous.back();
  }

 private:
  Node* root_ = new Node(false);
};

//
//
// ----------- Text Interface --------------------------------------------------

std::wstring tolower(const std::wstring& str) {
  std::wstring res;
  for (const auto& ch : str) {
    res.push_back(std::tolower(ch, std::locale()));
  }
  return res;
}

void InteractWithTextCommands(std::wistream& in, std::wostream& out) {
  RadixTrie trie{};
  std::wstring line;

  std::locale::global(std::locale(""));
  in.imbue(std::locale());
  out.imbue(std::locale());

  auto n = 0;
  std::wcin >> n;
  std::wcin.ignore();
  for (int i = 0; i < n; ++i) {
    std::getline(std::wcin, line);
    trie.Insert(tolower(line));
  }

  while (std::getline(in, line)) {
    if (line.empty()) continue;

    std::wstring lower_line = tolower(line);
    std::set<std::wstring> res = trie.FuzzySearch(lower_line);

    if (res.empty()) {
      std::wcout << line << " -?" << std::endl;
      continue;
    }
    if (res.find(lower_line) != res.end()) {
      std::wcout << line << " - ok" << std::endl;
    } else {
      std::wcout << line << " -> ";
      auto i = 0;
      for (const auto& w : res) {
        if (i != 0) std::wcout << ", ";
        std::wcout << w;
        i++;
      }
      std::wcout << std::endl;
    }
  }
}

int main() {
  InteractWithTextCommands(std::wcin, std::wcout);
  return 0;
}
