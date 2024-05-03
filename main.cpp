#include <boost/program_options.hpp>
#include <iostream>

#include "include/tape_sort.hpp"

void printHelp(const std::string& program_name,
               const boost::program_options::options_description& options) {
  std::cout << "Usage: " << program_name << " [options]\n";
  std::cout << options << '\n';
}

int main(int argc, char* argv[]) {
  boost::program_options::options_description options("Allowed options");
  options.add_options()("help,h", "Produce help message")(
      "input-filename,i",
      boost::program_options::value<std::string>()->required(),
      "Filename of input file tape")(
      "output-filename,o",
      boost::program_options::value<std::string>()->required(),
      "Filename of output file tape")(
      "delay-config-filename,d",
      boost::program_options::value<std::string>()->required(),
      "Filename of tape delay configuration")(
      "block-size,b",
      boost::program_options::value<size_t>()->default_value(50),
      "Max block size");

  boost::program_options::variables_map vm;
  try {
    boost::program_options::store(
        boost::program_options::parse_command_line(argc, argv, options), vm);
    if (vm.count("help")) {
      printHelp(argv[0], options);
      return 0;
    }
    boost::program_options::notify(vm);

    std::string input_tape_filename = vm["input-filename"].as<std::string>();
    std::string output_tape_filename = vm["output-filename"].as<std::string>();
    std::string delay_config_filename =
        vm["delay-config-filename"].as<std::string>();

    auto delay_config = DelayConfigLoader::load(delay_config_filename);

    TapeFileDevice input_tape(input_tape_filename, delay_config);
    TapeFileDevice output_tape(output_tape_filename, delay_config);

    const int block_size = vm["block-size"].as<size_t>();
    TapeSorter sorter(block_size,
                      std::make_unique<TmpTapeFileFactory>(delay_config));
    sorter.sort(input_tape, output_tape);

    int value;
    output_tape.rewind();
    while (output_tape.read(value)) {
      std::cout << value << ' ';
      output_tape.move(ITapeDevice::Direction::Forward);
    }
  } catch (const boost::program_options::error& e) {
    std::cerr << "ERROR: " << e.what() << "\n";
    printHelp(argv[0], options);
    return 1;
  } catch (const std::runtime_error& e) {
    std::cerr << "ERROR: " << e.what() << "\n";
    return 1;
  }

  return 0;
}