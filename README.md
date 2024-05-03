# Tape Sorting 
## Overview
This project is a C++ console application designed to simulate a data storage device that functions like a tape drive, capable of sequential data read and write operations. The main objective of this project is to sort integers from an input tape-file in ascending order and write the sorted data to an output tape-file under specific constraints regarding memory usage and operational efficiency.

Key attributes of the tape storage in this project include:

+ The tape can only move sequentially in either direction.
+ The read/write operations can only interact with the cell where the tape head is currently positioned.
+ Movement of the tape is time-consuming, underscoring the device's unsuitability for random access.
+ Given that the entire data on the tape cannot be accommodated in memory, the solution employs an external merge sort algorithm. This method involves the creation of multiple temporary tapes, which are then merged to form the sorted output on the destination tape.

## Build
1. Clone the repository:
```bash
git clone https://github.com/ALanovaya/tape-simulation.git
```
2. Install library for parsing arguments:
```bash
sudo apt update
sudo apt install -y libboost-program-options-dev
```
3. Build project:
```bash
make app
```

## Usage
```bash
Usage: ./app [options]
Allowed options:
  -h [ --help ]                      Produce help message
  -i [ --input-filename ] arg        Filename of input file tape
  -o [ --output-filename ] arg       Filename of output file tape
  -d [ --delay-config-filename ] arg Filename of tape delay configuration
  -b [ --block-size ] arg (=50)      Max block size
```

## Delay configuration file format
To emulate the delays associated with tape operations, a configuration file is utilized. These settings allow you to control the read, write, and movement delays to simulate different operational conditions. Example configuration:
```c
move_delay:20
read_delay:10
write_delay:10
rewind_delay:200
```
By default, all delays are zero.
## Running Tests
To run the unit tests, ensure Google Test is installed and set up on your system. Use the Makefile to compile and run tests:
```bash
make run_tests
./run_tests
```
## License
This project is licensed under the MIT License.
