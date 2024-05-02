#include <gtest/gtest.h>

#include "../include/delay_config.hpp"

class TestDelayConfig : public ::testing::Test {
  static constexpr const auto tmp_filename = "test_config";

protected:
  void TearDown() override { std::remove(tmp_filename); }

  void writeConfig(const std::stringstream &config_stream) {
    std::ofstream file(tmp_filename);
    file << config_stream.str();
  }

  std::string getFilename() { return tmp_filename; }
};

TEST_F(TestDelayConfig, load) {
  const int move_delay = 100;
  const int read_delay = 20;
  const int write_delay = 40;
  const int rewind_delay = 800;

  std::stringstream config_stream;
  config_stream << "move_delay:" << move_delay << '\n';
  config_stream << "read_delay:" << read_delay << '\n';
  config_stream << "write_delay:" << write_delay << '\n';
  config_stream << "rewind_delay:" << rewind_delay << '\n';
  writeConfig(config_stream);

  DelayConfig config = DelayConfigLoader{}.load(getFilename());
  ASSERT_EQ(config.move_delay.count(), move_delay);
  ASSERT_EQ(config.read_delay.count(), read_delay);
  ASSERT_EQ(config.write_delay.count(), write_delay);
  ASSERT_EQ(config.rewind_delay.count(), rewind_delay);
}

TEST_F(TestDelayConfig, emptyKey) {
  std::stringstream config_stream;
  config_stream << ":1";
  writeConfig(config_stream);
  ASSERT_THROW(DelayConfigLoader{}.load(getFilename()), std::invalid_argument);
}

TEST_F(TestDelayConfig, negValue) {
  std::stringstream config_stream;
  config_stream << "read_delay:-5";
  writeConfig(config_stream);
  ASSERT_THROW(DelayConfigLoader{}.load(getFilename()), std::invalid_argument);
}

TEST_F(TestDelayConfig, emptyValue) {
  std::stringstream config_stream;
  config_stream << "read_delay:a";
  writeConfig(config_stream);
  ASSERT_EQ(DelayConfigLoader{}.load(getFilename()).read_delay.count(), 0);
}
