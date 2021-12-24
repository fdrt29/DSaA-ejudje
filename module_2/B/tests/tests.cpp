// Copyright 2021 Fedor Teleshov <fdrt29@gmail.com>

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "../m2_taskB.hpp"

std::size_t number_of_files_in_directory(const std::filesystem::path& path) {
  using std::filesystem::directory_iterator;
  return std::distance(directory_iterator(path), directory_iterator{});
}

TEST(Example, EmptyTest) {
  std::filesystem::path path{"./module_2/B/tests/data/I"};
  if (not exists(path)) return;
  auto n = number_of_files_in_directory(path);
  for (size_t i = 1; i <= n; ++i) {
    std::ifstream in("./module_2/B/tests/data/I/" + std::to_string(i) + ".txt");
    EXPECT_TRUE(in.is_open());
    std::stringstream out;
    InteractWithDSByTextCommands(in, out);

    std::ifstream expected("./module_2/B/tests/data/O/" + std::to_string(i) +
                           ".txt");
    EXPECT_TRUE(expected.is_open());
    std::stringstream buffer;
    buffer << expected.rdbuf();
    std::string expected_str = buffer.str();
    std::string out_str = out.str();
    EXPECT_EQ(buffer.str(), out.str());
  }
}
