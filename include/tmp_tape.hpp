#pragma once

#include "tape.hpp"
#include <filesystem>
#include <memory>

class ITapeFactory {
public:
  virtual std::unique_ptr<ITapeDevice> create() const = 0;
  virtual ~ITapeFactory() = default;
};

class TmpTapeFileFactory : public ITapeFactory {
  DelayConfig config;

public:
  TmpTapeFileFactory(const DelayConfig &config = {}) : config(config) {}

  std::unique_ptr<ITapeDevice> create() const override {
    std::string tmp_dir = std::filesystem::temp_directory_path();
    std::string tmp_filename = tmp_dir + "/tapeXXXXXX";

    int fd = mkstemp(&tmp_filename[0]);
    if (fd == -1) {
      std::stringstream error_msg;
      error_msg << "Failed to create and open file: " << tmp_filename << '\n';
      throw std::ifstream::failure(error_msg.str());
    }
    close(fd);

    std::fstream tmp_file(tmp_filename, std::fstream::in | std::fstream::out |
                                            std::fstream::binary);
    if (!tmp_file.is_open()) {
      std::stringstream error_msg;
      error_msg << "Failed to open file: " << tmp_filename << '\n';
      throw std::ifstream::failure(error_msg.str());
    }
    tmp_file.close();

    return std::make_unique<TapeFileDevice>(tmp_filename, config);
  }
};