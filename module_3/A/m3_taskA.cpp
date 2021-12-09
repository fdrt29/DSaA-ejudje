#include <cstdint>
#include <iostream>
#include <regex>
#include <tuple>
#include <vector>

struct Item {
  Item(int weight, int cost) : weight(weight), cost(cost) {}
  int weight;
  int cost;
};

// TODO rework
std::vector<uint64_t> normWeights(size_t size, const std::vector<Item> &items) {
  uint64_t g = gcd(size, items[0].weight);
  for (uint64_t i = 1; i < items.size(); ++i) {
    g = gcd(g, items[i].weight);
  }
  if (g == 0) {
    g = 1;
  }
  std::vector<uint64_t> norm_weights(items.size());
  for (uint64_t i = 0; i < items.size(); ++i) {
    norm_weights[i] = items[i].weight / g;
  }
  norm_size_ /= g;
  return norm_weights;
}

uint64_t Backpack::gcd(uint64_t a, uint64_t b) {
  while (a && b) {
    if (a >= b) {
      a %= b;
    } else {
      b %= a;
    }
  }
  return a | b;
}

// 165
// 23 92
// 31 57
// 29 49
// 44 68
// 53 60
// 38 43
// 63 67
// 85 84
// 89 87
// 82 72
// end

// mb change int to something bigger?
std::tuple<int, int, std::vector<size_t>> alg(const std::vector<Item> &items,
                                              int limit) {
  std::vector<std::vector<int>> memo(items.size() + 1,
                                     std::vector<int>(limit + 1, 0));

  std::vector<uint64_t> norm_weights = normWeights(items);
  for (size_t i = 1; i <= items.size(); i++) {
    for (size_t j = 1; j <= limit; j++) {  // local limit
      if (items[i - 1].weight <= j) {
        memo[i][j] =
            std::max(memo[i - 1][j],
                     items[i - 1].cost + memo[i - 1][j - items[i - 1].weight]);
      } else {
        memo[i][j] = memo[i - 1][j];
      }
    }
  }

  //Обратный ход
  std::vector<size_t> res_ind;
  int res_weight = 0;
  int res_cost = 0;
  size_t j = limit;
  size_t i = items.size();
  while (i > 1 or j > 1) {
    if (memo[i][j] == 0) break;
    if (memo[i][j] != memo[i - 1][j]) {
      res_ind.push_back(i);
      res_weight += items[i - 1].weight;
      res_cost += items[i - 1].cost;
      j -= items[i - 1].weight;
    }
    i -= 1;
  }
  return std::make_tuple(res_weight, res_cost, res_ind);
}

inline void error() { std::cout << "error" << std::endl; }

int main() {
  int limit = 0;
  std::vector<Item> items;

  // Handle input
  bool is_limit_initialized = false;
  static const auto item_pattern = std::regex(R"(([+]?\d+)\s([+]?\d+)$)");
  std::smatch matches;
  std::string line;
  while (std::getline(std::cin, line)) {
    if (line == "end")  // TODO DELETE
      break;
    if (line == "\n" or line.empty()) continue;

    if (not is_limit_initialized) {
      try {
        limit = std::stoi(line);
        is_limit_initialized = true;
      } catch (std::invalid_argument const &e) {
        error();
      } catch (std::out_of_range const &e) {
        error();
      }
      continue;
    }

    if (std::regex_match(line, matches, item_pattern)) {
      items.emplace_back(std::stoi(matches[1]), std::stoi(matches[2]));
      continue;
    }
    // Else
    error();
  }

  // Algorithm
  auto [res_weight, res_cost, res_ind] = alg(items, limit);
  std::cout << res_weight << " " << res_cost << std::endl;

  std::sort(res_ind.begin(), res_ind.end());
  for (unsigned long i : res_ind) {
    std::cout << i << std::endl;
  }

  return 0;
}
