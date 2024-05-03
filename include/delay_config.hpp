#pragma once

#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

struct DelayConfig {
  std::chrono::milliseconds read_delay{};
  std::chrono::milliseconds write_delay{};
  std::chrono::milliseconds move_delay{};
  std::chrono::milliseconds rewind_delay{};
};

class DelayConfigLoader {
  constexpr static const auto move_delay_key = "move_delay";
  constexpr static const auto read_delay_key = "read_delay";
  constexpr static const auto write_delay_key = "write_delay";
  constexpr static const auto rewind_delay_key = "rewind_delay";

 public:
  static DelayConfig load(const std::string &filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
      std::stringstream error_msg;
      error_msg << "Failed to open file: " << filename << '\n';
      throw std::ifstream::failure(error_msg.str());
    }

    std::unordered_map<std::string, std::chrono::milliseconds> config_map;
    std::string line;
    std::string key;
    int64_t value;

    while (std::getline(file, line)) {
      std::istringstream iss(line);
      if (std::getline(iss, key, ':') && iss >> value) {
        if (key.empty()) {
          std::stringstream error_msg;
          error_msg << "Key is empty: " << line << '\n';
          throw std::invalid_argument(error_msg.str());
        }
        if (value < 0) {
          std::stringstream error_msg;
          error_msg << "Incorrect value < 0: " << line << '\n';
          throw std::invalid_argument(error_msg.str());
        }
        config_map[key] = std::chrono::milliseconds(value);
      }
    }

    DelayConfig config;

    auto get_delay_value = [&config_map](
                               const std::string &key,
                               std::chrono::milliseconds default_value =
                                   std::chrono::milliseconds(0)) {
      auto it = config_map.find(key);
      return it != config_map.end() ? it->second : default_value;
    };

    config.read_delay = get_delay_value(read_delay_key);
    config.write_delay = get_delay_value(write_delay_key);
    config.move_delay = get_delay_value(move_delay_key);
    config.rewind_delay = get_delay_value(rewind_delay_key);

    return config;
  }
};