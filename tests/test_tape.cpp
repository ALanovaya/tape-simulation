#include <algorithm>
#include <ctime>
#include <filesystem>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "../include/tape.hpp"

class TestTapeFileDevice : public ::testing::Test {
  static constexpr const auto tmp_filename = "test_tape";
  std::unique_ptr<TapeFileDevice> tape;

protected:
  void SetUp() override {
    std::ofstream(tmp_filename).close();
    tape = std::make_unique<TapeFileDevice>(tmp_filename, DelayConfig{});
  }

  void TearDown() override { std::remove(tmp_filename); }

  TapeFileDevice &getTape() { return *tape; }

  std::vector<int> readData() {
    std::vector<int> data;
    std::ifstream file(tmp_filename);
    int value;
    while (!file.read(reinterpret_cast<char *>(&value), sizeof(value)).eof()) {
      data.push_back(value);
    }
    return data;
  }

  void writeData(const std::vector<int> &data) {
    std::ofstream file(tmp_filename);
    for (auto value : data) {
      file.write(reinterpret_cast<char *>(&value), sizeof(int));
    }
  }
};

TEST_F(TestTapeFileDevice, writeRead) {
  auto &tape = getTape();
  int value = 90;
  int read_value;
  tape.write(value);
  ASSERT_TRUE(tape.read(read_value));
  ASSERT_EQ(value, read_value);
}

TEST_F(TestTapeFileDevice, multipleWrite) {
  const int amount = 100;
  std::vector<int> expected(amount);

  std::srand(unsigned(std::time(nullptr)));
  std::generate(expected.begin(), expected.end(), std::rand);
  auto &tape = getTape();

  for (int i = 0; i < amount; ++i) {
    tape.write(expected[i]);
    tape.move(TapeFileDevice::Direction::Forward);
  }
  ASSERT_EQ(expected, readData());
}

TEST_F(TestTapeFileDevice, writeSame) {
  int read_value;
  int value = 90;
  auto &tape = getTape();

  tape.write(value);
  value = 70;
  tape.write(value);
  tape.read(read_value);

  ASSERT_EQ(read_value, value);
}

TEST_F(TestTapeFileDevice, moveForward) {
  const int amount = 100;
  std::vector<int> expected(amount);

  std::srand(unsigned(std::time(nullptr)));
  std::generate(expected.begin(), expected.end(), std::rand);
  writeData(expected);

  auto &tape = getTape();
  std::vector<int> actual;
  int value;

  while (tape.read(value)) {
    actual.push_back(value);
    tape.move(TapeFileDevice::Direction::Forward);
  }
  ASSERT_EQ(expected, actual);
}

TEST_F(TestTapeFileDevice, moveBackward) {
  const int amount = 100;
  std::vector<int> expected(amount);

  std::srand(unsigned(std::time(nullptr)));
  std::generate(expected.begin(), expected.end(), std::rand);
  writeData(expected);
  std::reverse(expected.begin(), expected.end());

  auto &tape = getTape();
  std::vector<int> actual;
  int value;

  while (tape.read(value)) {
    tape.move(TapeFileDevice::Direction::Forward);
  }
  tape.move(TapeFileDevice::Direction::Backward);
  while (tape.read(value)) {
    actual.push_back(value);
    tape.move(TapeFileDevice::Direction::Backward);
  }

  ASSERT_EQ(expected, actual);
}

TEST_F(TestTapeFileDevice, moveBefore) {
  auto &tape = getTape();
  tape.move(TapeFileDevice::Direction::Backward);
  ASSERT_FALSE(tape.move(TapeFileDevice::Direction::Backward));
}

TEST_F(TestTapeFileDevice, readBefore) {
  int value;
  auto &tape = getTape();
  tape.move(TapeFileDevice::Direction::Backward);
  ASSERT_FALSE(tape.read(value));
}

TEST_F(TestTapeFileDevice, moveEnd) {
  auto &tape = getTape();
  ASSERT_FALSE(tape.move(TapeFileDevice::Direction::Forward));
}

TEST_F(TestTapeFileDevice, readEnd) {
  int value;
  auto &tape = getTape();
  ASSERT_FALSE(tape.read(value));
}

TEST_F(TestTapeFileDevice, moveBeforeRewind) {
  auto &tape = getTape();
  int value = 90;
  int read_value;

  tape.write(value);
  tape.move(TapeFileDevice::Direction::Backward);
  tape.rewind();
  ASSERT_TRUE(tape.read(read_value));
  ASSERT_EQ(value, read_value);
}

TEST_F(TestTapeFileDevice, rewind) {
  const int amount = 100;
  std::vector<int> expected(amount);

  std::srand(unsigned(std::time(nullptr)));
  std::generate(expected.begin(), expected.end(), std::rand);

  auto &tape = getTape();
  std::vector<int> actual;
  int value;

  for (int i = 0; i < amount; ++i) {
    tape.write(expected[i]);
    tape.move(TapeFileDevice::Direction::Forward);
  }
  tape.rewind();
  while (tape.read(value)) {
    actual.push_back(value);
    tape.move(TapeFileDevice::Direction::Forward);
  }
  ASSERT_EQ(expected, actual);
}

TEST_F(TestTapeFileDevice, writeBefore) {
  auto &tape = getTape();
  tape.move(TapeFileDevice::Direction::Backward);
  ASSERT_THROW(tape.write(1), std::out_of_range);
}