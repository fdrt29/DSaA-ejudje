#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>
#include <set>
#include <unordered_map>
#include <utility>
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
  struct Node {
    bool is_word_end = false;
    std::unordered_map<wchar_t, Node*> children;
    std::wstring label;

    explicit Node(std::wstring label = std::wstring(), bool word_end = false)
        : label(std::move(label)), is_word_end(word_end), children() {}

    ~Node() {
      for (auto const& el : children) {
        delete el.second;
      }
    }
  };

 public:
  ~RadixTrie() {
    delete root_;
  }

 public:
  /* Вставка.
   * Сложность по времени О(l), l - длина слова.
   * Соответствие первых букв слова и метке ребра находится за
   * константу(хэш-таблица). При каждом посещении вершины необходимо
   * итерироваться по суффиксу изначального слова для определения
   * максимального совпадающего с меткой префикса вплоть до конца
   * суффикса(переключаясь на дочерний узел если метка текущего узла - подстрока
   * суффикса) в худшем случае, в остальных - остаток суффикса просто необходимо
   * вставить.
   *
   * По памяти: O(1) - не зависит от входа.
   *
   * Вставляется 0-2 узла, когда найдено место.
   * */
  void Insert(std::wstring word) {
    Node* traverse_node = root_;

    while (true) {
      auto itr_edge = traverse_node->children.find(word[0]);
      if (itr_edge == traverse_node->children.end()) {
        // Traverse node hasn't matching node
        // Create new node marked {word}
        traverse_node->children[word[0]] = new Node(word, true);
        break;
      }

      auto& p_node = itr_edge->second;
      const auto& label = p_node->label;
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
        p_node->is_word_end = true;
        return;
      }
      // i_end points to next after match end
      auto i_end = MatchEndPosition(word, p_node->label);

      if (i_end == label.size() and i_end < word.size()) {
        word = word.substr(i_end);
        traverse_node = p_node;
        continue;

      } else if (i_end == word.size() and i_end < label.size()) {
        // Copy old node
        auto old_node = traverse_node->children[label[0]];
        // Create new node
        auto new_node = new Node(word, true);
        traverse_node->children[label[0]] = new_node;
        // Setup old node
        auto new_label = label.substr(i_end);
        old_node->label = new_label;
        // move old node to new node
        new_node->children[new_label[0]] = old_node;
        return;

      } else if (i_end < word.size() and i_end < label.size()) {
        auto prefix = word.substr(0, i_end);
        auto old_node = traverse_node->children[label[0]];

        auto new_inner_node = new Node(prefix, false);
        traverse_node->children[label[0]] = new_inner_node;
        // Move old node to inner node
        old_node->label = label.substr(i_end);
        new_inner_node->children[old_node->label[0]] = old_node;
        // Create new node from inner node to new node
        new_inner_node->children[word[i_end]] =
            new Node(word.substr(i_end), true);
        return;
      }
    }
  }

  /* Нечеткий поиск.
   * По времени: О(n*l), где n - количество узлов(или количество символов
   * хранимых деревом если точнее) в дереве, l - длина искомого слова. Всего
   * создается строк в таблице не больше, чем символов во всех проверенных
   * префиксном пути. Длина строк - l. В худшем случае придется пройти по всем
   * узлам и символам. Худший случай: расстояние всех итоговых путей в дереве до
   * искомого слова равно 1.
   *
   * По памяти: О((l+1)*l)=O(l^2)=O(n*l). Для каждого узла будет храниться 3
   * строки таблицы длиной l + доп память на фрейм. Худший случай: l+1 узлов
   * уходят в глубину и каждый равен соответствующей букве слова, последний -
   * конец слова с дополнительной буквой относительно искомого(на каждом уровне
   * имеются какие-то другие дети, чтобы путь был не сжатым). Стек вызовов будет
   * содержать l+1 фреймов рекурсивной функции, каждый будет хранить 3 строки
   * длинной l.
   * */
  std::set<std::wstring> FuzzySearch(const std::wstring& word,
                                     uint max_mistake_count = 1) {
    std::set<std::wstring> results;

    auto m = word.length() + 1;
    std::vector<uint> pre_previous(m);
    std::vector<uint> previous(m);
    std::iota(previous.begin(), previous.end(), 0);

    for (auto& [ch, p_node] : root_->children) {
      RecursiveFuzzySearch(p_node, pre_previous, previous, results,
                           root_->label, word, max_mistake_count);
    }

    return results;
  }

 private:
  static void RecursiveFuzzySearch(Node* p_current_node,
                                   std::vector<uint> pre_previous,
                                   std::vector<uint> previous,
                                   std::set<std::wstring>& results,
                                   const std::wstring& prefix,
                                   const std::wstring& word,
                                   uint max_mistake_count = 1) {
    std::wstring current_labels_path = prefix + p_current_node->label;

    // Similar word and input word lengths may differ by no more than the
    // max mistake count. Because a possible mistake is inserting an extra
    // character
    int length_difference = static_cast<int>(current_labels_path.length()) -
                            static_cast<int>(word.length());
    if (length_difference > static_cast<int>(max_mistake_count)) return;

    bool prev_has_valid_mistakes_count = false;
    // Достраиваем таблицу
    for (auto i = prefix.size() + 1; i < current_labels_path.length() + 1;
         i++) {
      std::vector<uint> current(word.length() + 1);
      current[0] = i;
      bool curr_has_valid_mistakes_count = false;
      for (auto j = 1; j < word.length() + 1; j++) {
        uint cost = current_labels_path[i - 1] == word[j - 1] ? 0 : 1;

        uint insert = previous[j] + 1;
        uint del = current[j - 1] + 1;
        uint replace = previous[j - 1] + cost;

        current[j] = std::min({insert, del, replace});

        if (i > 1 && j > 1 && current_labels_path[i - 1] == word[j - 2] &&
            current_labels_path[i - 2] == word[j - 1]) {
          // Transposition
          current[j] = std::min(current[j], pre_previous[j - 2] + cost);
        }
        if (not curr_has_valid_mistakes_count and
            current[j] <= max_mistake_count)
          curr_has_valid_mistakes_count = true;
      }
      if (not curr_has_valid_mistakes_count and prev_has_valid_mistakes_count)
        return;  // Early termination of algorithm

      pre_previous = previous;
      previous = current;
      prev_has_valid_mistakes_count = curr_has_valid_mistakes_count;
    }

    if (previous.back() <= max_mistake_count and p_current_node->is_word_end) {
      results.insert(current_labels_path);
    }

    for (auto& [ch, p_node] : p_current_node->children) {
      RecursiveFuzzySearch(p_node, pre_previous, previous, results,
                           current_labels_path, word, max_mistake_count);
    }
  }

 private:
  Node* root_ = new Node();
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
