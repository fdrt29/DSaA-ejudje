#include <cstdint>
#include <vector>

struct Item {
  Item(int weight, int cost) : weight(weight), cost(cost) {}
  int weight;
  int cost;
};

// mb change int to something bigger?
std::tuple<int, int, std::vector<size_t>> alg(const std::vector<Item>& items,
                                              int limit) {
  std::vector<std::vector<int>> memo(items.size() + 1,
                                     std::vector<int>(limit + 1, 0));
  for (size_t i = 1; i <= items.size() + 1; i++) {
    for (size_t j = 1; j <= limit + 1; j++) {  // local limit
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

int main() {
  while (std::getline(std::cin, line)) {
  }
  return 0;
}
