#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <limits>

struct Item {
    uint64_t weight;
    uint64_t cost;

    Item(uint64_t weight, uint64_t cost) : weight(weight), cost(cost) {
    }
};

class Backpack {
public:
    explicit Backpack(uint64_t size);
    ~Backpack() = default;

    void pack(const std::vector<uint64_t>&, const std::vector<uint64_t>&);
    void pack(std::vector<Item>& items);

    std::vector<uint64_t> getNumbers() const { return numbers_; }
    uint64_t getTotalWeight() const { return total_weight_; }
    uint64_t getTotalCost() const { return total_cost_; }

private:
    uint64_t size_;
    uint64_t norm_size_;
    uint64_t total_weight_;
    uint64_t total_cost_;
    std::vector<Item> items_;
    std::vector<uint64_t> numbers_;

    static uint64_t gcd(uint64_t, uint64_t);
    std::vector<uint64_t> normWeights(const std::vector<Item>&);
    void findNumbers(const std::vector<std::vector<uint64_t>>&, uint64_t, uint64_t,
                     const std::vector<Item>&, const std::vector<uint64_t>&);
};

Backpack::Backpack(uint64_t size) {
    size_ = size;
    norm_size_ = size;
    total_weight_ = 0;
    total_cost_ = 0;
}

void Backpack::pack(std::vector<Item>& items) {
    if (items.size() == 0) {
        return;
    }
    std::vector<uint64_t> norm_weights = normWeights(items);
    std::vector<std::vector<uint64_t>> dp(items.size() + 1, std::vector<uint64_t>(norm_size_ + 1, 0));
    for (uint64_t i = 1; i <= items.size(); ++i) {
        for (uint64_t j = 0; j <= norm_size_; ++j) {
            if (norm_weights[i - 1] <= j) {
                if (dp[i - 1][j] >= dp[i - 1][j - norm_weights[i - 1]] + items[i - 1].cost) {
                    dp[i][j] = dp[i - 1][j];
                } else {
                    dp[i][j] = dp[i - 1][j - norm_weights[i - 1]] + items[i - 1].cost;
                }
            } else {
                dp[i][j] = dp[i - 1][j];
            }
        }
    }
    total_cost_ = dp[items.size()][norm_size_];
    findNumbers(dp, items.size(), norm_size_, items, norm_weights);
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

std::vector<uint64_t> Backpack::normWeights(const std::vector<Item>& items) {
    uint64_t g = gcd(size_, items[0].weight);
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

void Backpack::findNumbers(const std::vector<std::vector<uint64_t>>& dp, uint64_t number,
                           uint64_t col, const std::vector<Item>& items,
                           const std::vector<uint64_t>& norm_weights) {
    if (dp[number][col] == 0) {
        return;
    }
    if (dp[number - 1][col] == dp[number][col]) {
        findNumbers(dp, number - 1, col, items, norm_weights);
    } else {
        findNumbers(dp, number - 1, col - norm_weights[number - 1], items, norm_weights);
        total_weight_ += items[number - 1].weight;
        numbers_.push_back(number);
        items_.push_back(items[number - 1]);
    }
}

void skipSpaces(const std::string& line, uint64_t& pos) {
    while (pos < line.size() && std::isspace(line[pos])) {
        ++pos;
    }
}

uint64_t getUint64_t(const std::string& line, uint64_t& pos) {
    uint64_t number = 0;
    while (pos < line.size() && std::isdigit(line[pos])) {
        number = number * 10 + (line[pos] - '0');
        ++pos;
    }
    return number;
}

bool getOneUint64_t(const std::string& line, uint64_t& number) {
    uint64_t pos = 0;
    skipSpaces(line, pos);
    if (pos == line.size() || !std::isdigit(line[pos])) {
        return false;
    }
    number = getUint64_t(line, pos);
    skipSpaces(line, pos);
    return pos == line.size();
}

bool getTwoUint64_t(const std::string& line, uint64_t& number_one, uint64_t& number_two) {
    uint64_t pos = 0;
    skipSpaces(line, pos);
    if (pos == line.size() || !std::isdigit(line[pos])) {
        return false;
    }
    number_one = getUint64_t(line, pos);
    skipSpaces(line, pos);
    if (pos == line.size() || !std::isdigit(line[pos])) {
        return false;
    }
    number_two = getUint64_t(line, pos);
    skipSpaces(line, pos);
    return pos == line.size();
}

uint64_t getBackpackSize() {
    uint64_t backpack_size = 0;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.size() == 0) {
            continue;
        }
        if (getOneUint64_t(line, backpack_size)) {
            return backpack_size;
        }
        std::cout << "error\n";
    }
    return std::numeric_limits<uint64_t>::max();
}

std::vector<Item> getWeightsCosts() {
    std::string line;
    uint64_t weight, cost;
    std::vector<Item> items;
    while (std::getline(std::cin, line)) {
        if (line.size() == 0) {
            continue;
        }
        if (getTwoUint64_t(line, weight, cost)) {
            items.push_back(Item(weight, cost));
        } else {
            std::cout << "error\n";
        }
    }
    return items;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    uint64_t backpack_size = getBackpackSize();
    if (backpack_size == std::numeric_limits<uint64_t>::max()) {
        return 0;
    }

    std::vector<Item> items = getWeightsCosts();

    Backpack backpack(backpack_size);
    backpack.pack(items);
    
    std::cout << backpack.getTotalWeight() << " " << backpack.getTotalCost() << "\n";
    for (uint64_t number : backpack.getNumbers()) {
        std::cout << number << "\n";
    }

    return 0;
}