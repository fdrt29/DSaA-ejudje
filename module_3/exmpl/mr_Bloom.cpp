#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <cmath>

class BitArray {
public:
    explicit BitArray(uint64_t);
    ~BitArray();

    void setBit(uint64_t);
    bool isBitOne(uint64_t) const;
    std::string toString() const;

private:
    static const uint64_t kTypeSize = sizeof(uint64_t) * 8;

    uint64_t* buffer_;
    uint64_t number_bits_;
    uint64_t size_;
};

BitArray::BitArray(uint64_t number_bits) {
    number_bits_ = number_bits;
    size_ = (number_bits_ + kTypeSize - 1ULL) / kTypeSize;
    buffer_ = new uint64_t[size_];
    for (uint64_t i = 0; i < size_; ++i) {
        buffer_[i] = 0;
    }
}

BitArray::~BitArray() {
    delete[] buffer_;
}

void BitArray::setBit(uint64_t index) {
    uint64_t int_index = index / kTypeSize;
    uint64_t bit_index = index % kTypeSize;
    buffer_[int_index] |= (1ULL << (kTypeSize - 1ULL - bit_index));
}

bool BitArray::isBitOne(uint64_t index) const {
    uint64_t int_index = index / kTypeSize;
    uint64_t bit_index = index % kTypeSize;
    return (buffer_[int_index] | (1ULL << (kTypeSize - 1ULL - bit_index))) == buffer_[int_index];
}

std::string BitArray::toString() const {
    std::string bits;
    bits.reserve(number_bits_);
    for (uint64_t i = 0; i < size_; ++i) {
        uint64_t bit_number = kTypeSize;
        while (bits.size() < number_bits_ && bit_number > 0) {
            uint64_t bit = buffer_[i] & (1ULL << (bit_number - 1ULL));
            bits += (bit ? '1' : '0');
            --bit_number;
        }
    }
    return bits;
}

class BloomFilterException {
public:
    BloomFilterException(const char* msg) {
        msg_ = msg;
    }
    BloomFilterException(const std::string& msg) {
        msg_ = msg;
    }

    std::string& getMessage() {
        return msg_;
    }
private:
    std::string msg_;
};

class BloomFilter {
public:
    BloomFilter(uint64_t, double);
    ~BloomFilter();

    void add(uint64_t);
    bool search(uint64_t) const;

    inline uint64_t getSize() const { return size_; }
    inline uint64_t getNumberHashes() const { return number_hashes_; }

    friend std::ostream& operator<<(std::ostream&, const BloomFilter&);

private:
    static const uint64_t kMersenThirtyOne = 2147483647;

    uint64_t number_hashes_;
    uint64_t size_;
    std::vector<uint64_t> prime_numbers_;
    BitArray* bit_array_;

    uint64_t hash_(uint64_t, uint64_t) const;
    std::vector<uint64_t> getPrimeNumbers() const;
};

BloomFilter::BloomFilter(uint64_t size, double p) {
    number_hashes_ = (uint64_t)round(-log2(p));
    if (number_hashes_ == 0) {
        throw BloomFilterException("error");
    }
    size_ = (uint64_t)round(size * (-log2(p) / log(2)));
    prime_numbers_ = getPrimeNumbers();
    bit_array_ = new BitArray(size_);
}

BloomFilter::~BloomFilter() {
    delete bit_array_;
}

void BloomFilter::add(uint64_t key) {
    for (uint64_t i = 0; i < number_hashes_; ++i) {
        bit_array_->setBit(hash_(i, key));
    }
}

bool BloomFilter::search(uint64_t key) const {
    for (uint64_t i = 0; i < number_hashes_; ++i) {
        if (!bit_array_->isBitOne(hash_(i, key))) {
            return false;
        }
    }
    return true;
}

std::vector<uint64_t> BloomFilter::getPrimeNumbers() const {
    std::vector<uint64_t> prime_numbers;
    prime_numbers.push_back(2);
    uint64_t test_number = 3;
    while (prime_numbers.size() < number_hashes_) {
        bool is_prime = true;
        for (uint64_t prime_number : prime_numbers) {
            if (test_number % prime_number == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) {
            prime_numbers.push_back(test_number);
        }
        test_number += 2;
    }
    return prime_numbers;
}

uint64_t BloomFilter::hash_(uint64_t i, uint64_t x) const {
    x %= kMersenThirtyOne;
    return (((1 + i) * x + prime_numbers_[i]) % kMersenThirtyOne) % size_;
}

std::ostream& operator<<(std::ostream& out, const BloomFilter& filter) {
    out << filter.bit_array_->toString();
    return out;
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

bool getOneUint64_t(const std::string& line, uint64_t& number, uint64_t& pos) {
    skipSpaces(line, pos);
    if (pos == line.size() || !std::isdigit(line[pos])) {
        return false;
    }
    number = getUint64_t(line, pos);
    skipSpaces(line, pos);
    return true;
}

BloomFilter* getBloomFilter() {
    BloomFilter* filter;
    std::string line;
    std::string op;
    uint64_t n;
    uint64_t pos = 4;
    double p;
    while (std::getline(std::cin, line)) {
        if (line.size() == 0) {
            continue;
        }
        op = line.substr(0, 4);
        if (op != "set ") {
            std::cout << "error\n";
            continue;
        }
        pos = 4;
        if (!getOneUint64_t(line, n, pos) || pos == line.size() || n == 0) {
            std::cout << "error\n";
            continue;
        }
        op = line.substr(pos);
        try {
            p = std::stod(op);
        }
        catch (...) {
            std::cout << "error\n";
            continue;
        }
        if (p <= 0 || p >= 1) {
            std::cout << "error\n";
            continue;
        }
        try {
            filter = new BloomFilter(n, p);
        }
        catch (const BloomFilterException&) {
            std::cout << "error\n";
            continue;
        }
        std::cout << filter->getSize() << " " << filter->getNumberHashes() << "\n";
        return filter;
    }
    return nullptr;
}

void execCommands(BloomFilter* filter) {
    std::string line;
    std::string op;
    uint64_t pos, key;
    while (std::getline(std::cin, line)) {
        if (line.size() == 0) {
            continue;
        }
        if (line == "print") {
            std::cout << *filter << "\n";
            continue;
        }
        op = line.substr(0, 4);
        if (op == "add ") {
            pos = 4;
            if (!getOneUint64_t(line, key, pos) || pos != line.size()) {
                std::cout << "error\n";
                continue;
            }
            filter->add(key);
            continue;
        }
        op = line.substr(0, 7);
        if (op == "search ") {
            pos = 7;
            if (!getOneUint64_t(line, key, pos) || pos != line.size()) {
                std::cout << "error\n";
                continue;
            }
            std::cout << (filter->search(key) ? "1\n" : "0\n");
            continue;
        }
        std::cout << "error\n";
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    BloomFilter* filter = getBloomFilter();
    if (filter == nullptr) {
        return 0;
    }
    execCommands(filter);
    delete filter;

    return 0;
}