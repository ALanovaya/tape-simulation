#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <queue>
#include <vector>

#include "tmp_tape.hpp"

class BlockOperations {
 public:
  static std::vector<int> readBlock(ITapeDevice& tape, int blockSize) {
    std::vector<int> block;
    int value;

    for (int i = 0; i < blockSize && tape.read(value); ++i) {
      block.push_back(value);
      tape.move(ITapeDevice::Direction::Forward);
    }
    return block;
  }

  static void writeBlock(ITapeDevice& tape, const std::vector<int>& block) {
    for (int value : block) {
      tape.write(value);
      tape.move(ITapeDevice::Direction::Forward);
    }
  }
};

class TapeSorter {
  int max_block_size;
  std::unique_ptr<ITapeFactory> tmp_tape_factory;

  std::vector<std::unique_ptr<ITapeDevice>> split(
      ITapeDevice& input_tape) const {
    std::vector<std::unique_ptr<ITapeDevice>> subtapes;
    int value;

    while (input_tape.read(value)) {
      std::vector<int> block =
          BlockOperations::readBlock(input_tape, max_block_size);
      auto tmp_tape = tmp_tape_factory->create();

      std::sort(block.begin(), block.end(), std::greater<int>());
      BlockOperations::writeBlock(*tmp_tape, block);
      tmp_tape->move(ITapeDevice::Direction::Backward);
      subtapes.push_back(std::move(tmp_tape));
    }
    return subtapes;
  }

  void merge(std::vector<std::unique_ptr<ITapeDevice>>& subtapes,
             ITapeDevice& output_tape) const {
    auto comp = [](const std::unique_ptr<ITapeDevice>& a,
                   const std::unique_ptr<ITapeDevice>& b) {
      int value_a, value_b;
      a->read(value_a);
      b->read(value_b);
      return value_a > value_b;
    };

    std::priority_queue<std::unique_ptr<ITapeDevice>,
                        std::vector<std::unique_ptr<ITapeDevice>>,
                        decltype(comp)>
        tape_queue(comp);
    for (auto& tape : subtapes) {
      int value;
      if (tape->read(value)) {
        tape_queue.push(std::move(tape));
      }
    }

    while (!tape_queue.empty()) {
      int value;
      auto min_tape = std::move(
          const_cast<std::unique_ptr<ITapeDevice>&>(tape_queue.top()));
      tape_queue.pop();

      min_tape->read(value);
      output_tape.write(value);
      output_tape.move(ITapeDevice::Direction::Forward);
      min_tape->move(ITapeDevice::Direction::Backward);

      if (min_tape->read(value)) {
        tape_queue.push(std::move(min_tape));
      }
    }
  }

 public:
  TapeSorter(int max_block_size,
             std::unique_ptr<ITapeFactory> tmp_tape_factory =
                 std::make_unique<TmpTapeFileFactory>())
      : max_block_size(max_block_size),
        tmp_tape_factory(std::move(tmp_tape_factory)) {}

  void sort(ITapeDevice& input_tape, ITapeDevice& output_tape) {
    auto subtapes = split(input_tape);
    merge(subtapes, output_tape);
  }
};