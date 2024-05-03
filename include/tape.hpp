#pragma once

#include <fstream>
#include <thread>
#include <unordered_map>

#include "delay_config.hpp"
#include "tape_interface.hpp"

class TapeFileDevice : public ITapeDevice {
  std::fstream tape_file;
  std::streampos current_position = std::fstream::beg;
  DelayConfig config;

public:
  TapeFileDevice(const std::string &filename, const DelayConfig &config)
      : tape_file(filename,
                  std::fstream::in | std::fstream::out | std::fstream::binary),
        config(config) {
    if (!tape_file.is_open()) {
      std::stringstream error_msg;
      error_msg << "Failed to open file: " << filename << '\n';
      throw std::ifstream::failure(error_msg.str());
    }
  }

  bool read(int &value) override {
    std::this_thread::sleep_for(config.read_delay);
    if (is_before())
      return false;
    tape_file.read(reinterpret_cast<char *>(&value), sizeof(int));
    if (tape_file.eof()) {
      tape_file.clear();
      return false;
    }
    update_position();
    return true;
  }

  void write(int value) override {
    std::this_thread::sleep_for(config.write_delay);
    if (is_before())
      throw std::out_of_range("Cannot write before the beginning of the tape");
    tape_file.write(reinterpret_cast<char *>(&value), sizeof(int));
    update_position();
  }

  void rewind() override {
    std::this_thread::sleep_for(config.rewind_delay);
    tape_file.clear();
    if (!is_before())
      update_position(-std::streamoff(current_position));
    else
      update_position(std::streamoff(sizeof(int)));
  }

  bool move(Direction direction) override {
    std::this_thread::sleep_for(config.move_delay);
    if (direction == Direction::Backward && is_before())
      return false;
    else if (direction == Direction::Forward && tape_file.peek(),
             tape_file.eof())
      return false;
    update_position(direction == Direction::Forward
                        ? std::streamoff(sizeof(int))
                        : -std::streamoff(sizeof(int)));
    return true;
  }

private:
  bool is_before() { return current_position < std::fstream::beg; }

  void update_position(std::streamoff shift = std::streamoff(0)) {
    current_position += shift;
    tape_file.seekg(current_position);
    tape_file.seekp(current_position);
  }
};