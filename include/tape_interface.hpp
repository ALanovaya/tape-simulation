#pragma once

class ITapeDevice {
 public:
  enum class Direction { Forward, Backward };

  virtual bool read(int &value) = 0;
  virtual void write(int value) = 0;
  virtual bool move(Direction direction) = 0;
  virtual void rewind() = 0;
  virtual ~ITapeDevice() = default;
};