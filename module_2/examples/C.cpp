#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <string>

using namespace std;

class min_heat final {
 public:
  min_heat() = default;
  ~min_heat() = default;

  pair<size_t , string> search(const long long key) const {
    if (hash_table.find(key) == hash_table.end()) {
      throw runtime_error("error");
    }
    return make_pair(hash_table.at(key), heap[hash_table.at(key)].second);
  }

  void add(const long long key, const string& value) {
    if (hash_table.find(key) != hash_table.end()) {
      throw runtime_error("error");
    }
    heap.emplace_back(key, value);
    hash_table.insert(make_pair(key, heap.size() - 1));
    lifting(key);
  }

  void set(const long long key, const string& value) {
    if (hash_table.find(key) == hash_table.end()) {
      throw runtime_error("error");
    }
    heap[hash_table.at(key)].second = value;
  }

  void erase(const long long key) {
    if (hash_table.find(key) == hash_table.end()) {
      throw runtime_error("error");
    }
    if (heap.back().first == key){
      hash_table.erase(key);
      heap.pop_back();
      return;
    }
    auto key_new_node = heap.back().first;
    heap[hash_table.at(key)].swap(heap.back());
    swap(hash_table.at(key), hash_table.at(key_new_node));
    hash_table.erase(key);
    heap.pop_back();
    lifting(key_new_node);
    heapify(key_new_node);
  }

  pair<size_t, pair<long long, string>> min() const {
    if (heap.empty()) {
      throw runtime_error("error");
    }
    return make_pair(0, heap.front());
  }

  pair<size_t, pair<long long , string>> max() const {
    if (heap.empty()) {
      throw runtime_error("error");
    }
    auto max = heap[heap.size() / 2].first;
    for (int i = int(heap.size() / 2 + 1); i < heap.size(); ++i) {
      if (max < heap[i].first)
        max = heap[i].first;
    }
    return make_pair(hash_table.at(max), heap[hash_table.at(max)]);
  }

  pair<long long, string> extract() {
    if (heap.empty()) {
      throw runtime_error("error");
    }
    auto key_new_node = heap.back().first;
    heap.front().swap(heap.back());
    swap(hash_table.at(heap.back().first), hash_table.at(key_new_node));
    hash_table.erase(heap.back().first);
    pair<long long , string> extract_ = heap.back();
    heap.pop_back();
    lifting(key_new_node);
    heapify(key_new_node);
    return extract_;
  }

  friend void print(const min_heat& tree, ostream& out);

 private:
  unordered_map<long long ,size_t> hash_table;
  vector<pair<long long ,string>> heap;

 private:
  bool swap_(const long long key, const int parity, bool flag) {
    if (heap[hash_table.at(key)].first <
        heap[(hash_table.at(key) - parity) / 2].first) {
      heap[hash_table.at(key)].swap(heap[(hash_table.at(key) - parity) / 2]);
      swap(hash_table.at(key), hash_table.at(heap[hash_table.at(key)].first));
      flag = true;
    } else {
      flag = false;
    }
    return flag;
  }

  void heapify(const long long key) {
    bool flag = true;
    while (flag) {
      size_t Left = 2 * hash_table.at(key) + 1;
      size_t Right = 2 * hash_table.at(key) + 2;
      if (Left < heap.size() && Right < heap.size()){
        if (heap[Left].first < key && heap[Left].first < heap[Right].first)
          flag = swap_(heap[Left].first, 1, flag);
        else if (heap[Right].first < key) {
          flag = swap_(heap[Right].first, 2, flag);
        } else {
          return;
        }
      } else if (Left < heap.size() && heap[Left].first < key) {
        flag = swap_(heap[Left].first, 1, flag);
      }
      else if (Right < heap.size() && heap[Right].first < key) {
        flag = swap_(heap[Right].first, 2, flag);
      }
      else return;
    }
  }

  void lifting(const long long key) {
    bool flag = true;
    while (flag) {
      if (hash_table.at(key) == 0) {
        return;
      }
      if (hash_table.at(key) % 2 == 0) {
        flag = swap_(key, 2, flag);

      } else if (hash_table.at(key) % 2 == 1) {
        flag = swap_(key, 1, flag);
      }
    }
  }
};

void print(const min_heat& tree, ostream& out) {
  if (tree.heap.empty()) {
    out << "_" << endl;
    return;
  }

  out << "[" << tree.heap[0].first
      << " " << tree.heap[0].second << "]" << " " << endl;

  size_t level = 2;
  size_t counter = 0;

  for (size_t i = 1; i < tree.heap.size(); ++i) {
    out << "[" << tree.heap[i].first << " "
        << tree.heap[i].second << " "
        << tree.heap[(i - 1) / 2].first << "]" << " ";

    counter++;
    if (i + 1 == tree.heap.size()) {
      for (size_t j = 0; j < level - counter; ++j) {
        out << "_" << " ";
      }
      out << endl;
      break;
    }
    if (counter == level) {
      counter = 0;
      level <<= 1;
      out << endl;
    }
  }
}

int main(){
  min_heat min_heat_;
  while (!cin.eof()) {
    string string_, date;
    long long key;
    cin >> string_;
    try {
      if (string_.empty()) {
        continue;
      } else if (string_ == "add") {
        cin >> key >> date;
        min_heat_.add(key, date);
      } else if (string_ == "set") {
        cin >> key >> date;
        min_heat_.set(key, date);
      } else if (string_ == "delete") {
        cin >> key;
        min_heat_.erase(key);
      } else if (string_ == "search") {
        cin >> key;
        try {
          pair<long long, string> search = min_heat_.search(key);
          cout << "1 " << search.first << " " << search.second << "\n";
        } catch (...) {
          cout << "0\n";
        }
      } else if (string_ == "min") {
        auto min = min_heat_.min();
        cout << min.second.first << " " << min.first << " "
             << min.second.second << "\n";
      } else if (string_ == "max") {
        pair<size_t, pair<long long, string>> max = min_heat_.max();
        cout << max.second.first << " " << max.first << " "
             << max.second.second << "\n";

      } else if (string_ == "extract") {
        auto max = min_heat_.extract();
        cout << max.first << " " << max.second << "\n";
      } else if (string_ == "print") {
        print(min_heat_, cout);
      } else {
        cout << "error\n";
      }
    } catch (...) {
      cout << "error\n";
    }
    string_.clear();
  }
  return 0;
}
