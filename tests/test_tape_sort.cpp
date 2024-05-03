#include <gtest/gtest.h>
#include <random>

#include "../include/tape_sort.hpp"

class TestTapeSort : public testing::TestWithParam<std::tuple<int, int>> {
  static constexpr const auto tmp_filename = "test_tape_sort";
  std::unique_ptr<TapeFileDevice> input_tape;
  std::unique_ptr<TapeFileDevice> output_tape;

protected:
  void SetUp() override {
    std::ofstream(tmp_filename).close();
    input_tape = std::make_unique<TapeFileDevice>(tmp_filename, DelayConfig{});
    output_tape = std::make_unique<TapeFileDevice>(tmp_filename, DelayConfig{});
  }

  void TearDown() override { std::remove(tmp_filename); }

  TapeFileDevice &getInputTape() { return *input_tape; }

  TapeFileDevice &getOutputTape() { return *output_tape; }

  std::vector<int> readDataFromOutputTape() {
    std::ifstream file(tmp_filename);
    std::vector<int> data;
    int value;

    while (!file.read(reinterpret_cast<char *>(&value), sizeof(value)).eof()) {
      data.push_back(value);
    }
    return data;
  }

  void writeDataToInputTape(const std::vector<int> &data) {
    std::ofstream file(tmp_filename);
    for (auto value : data) {
      file.write(reinterpret_cast<char *>(&value), sizeof(int));
    }
  }
};

TEST_P(TestTapeSort, randomValues) {
  int amount = std::get<0>(GetParam());
  int block_size = std::get<1>(GetParam());

  std::vector<int> expected(amount);
  std::srand(unsigned(std::time(nullptr)));
  std::generate(expected.begin(), expected.end(), std::rand);
  writeDataToInputTape(expected);

  auto &input_tape = getInputTape();
  auto &output_tape = getOutputTape();

  TapeSorter(block_size).sort(input_tape, output_tape);
  std::sort(expected.begin(), expected.end());
  ASSERT_EQ(readDataFromOutputTape(), expected);
}

TEST_F(TestTapeSort, sortedValues) {
  int amount = 1000;
  int block_size = 10;

  std::vector<int> expected(amount);
  std::iota(expected.begin(), expected.end(), 0);
  std::vector<int> reversed(expected.rbegin(), expected.rend());
  writeDataToInputTape(reversed);

  auto &input_tape = getInputTape();
  auto &output_tape = getOutputTape();

  TapeSorter(block_size).sort(input_tape, output_tape);
  ASSERT_EQ(readDataFromOutputTape(), expected);
}

INSTANTIATE_TEST_SUITE_P(Sort, TestTapeSort,
                         testing::Combine(testing::Values(1, 100, 10'000,
                                                          1'000'000),
                                          testing::Values(10, 50, 100, 500)));
