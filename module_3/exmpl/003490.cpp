#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <limits>
#include <algorithm>

size_t countZeroes(uint64_t number) {
    size_t count = 0;
    while ((number & 1) == 0) {
        number >>= 1;
        ++count;
    }
    return count;
}

std::vector<char> getSteps(uint64_t half_money) {
    const char kInc = 0;
    const char kDec = 1;
    const char kDbl = 2;

    std::vector<char> steps;
    uint64_t money = half_money;
    while (money) {
        if ((money & 1) == 1) {
            if (money > 3 && countZeroes(money + 1) > countZeroes(money - 1)) {
                steps.push_back(kDec);
                ++money;
            } else {
                steps.push_back(kInc);
                --money;
            }
        } else {
            steps.push_back(kDbl);
            money >>= 1;
        }
    }
    std::reverse(steps.begin(), steps.end());
    return steps;
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

uint64_t getNumber() {
    uint64_t number = 0;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.size() == 0) {
            continue;
        }
        if (getOneUint64_t(line, number)) {
            return number;
        }
        std::cout << "error\n";
    }
    return std::numeric_limits<uint64_t>::max();
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const std::vector<std::string> commands = {"inc", "dec", "dbl" };

    uint64_t half_money = getNumber();
    if (half_money == std::numeric_limits<uint64_t>::max()) {
        return 0;
    }

    std::vector<char> steps = getSteps(half_money);
    for (char step : steps) {
        std::cout << commands[step] << "\n";
    }

    return 0;
}