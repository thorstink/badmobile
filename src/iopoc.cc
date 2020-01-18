#include <chrono>
#include <cstdlib>
#include <gpiod.hpp>
#include <iostream>
#include <thread>

int main(int argc, char **argv) {
  if (argc < 3) {
    ::std::cerr << "usage: " << argv[0] << " <chip> <line_offset0>=<value0> ..."
                << ::std::endl;
    return EXIT_FAILURE;
  }

  ::std::vector<unsigned int> offsets;
  ::std::vector<int> values;
  for (int i = 2; i < argc; i++) {
    ::std::string arg(argv[i]);

    size_t pos = arg.find('=');

    ::std::string offset(arg.substr(0, pos));
    ::std::string value(arg.substr(pos + 1, ::std::string::npos));

    if (offset.empty() || value.empty())
      throw ::std::invalid_argument("invalid argument: " +
                                    ::std::string(argv[i]));

    offsets.push_back(::std::stoul(offset));
    values.push_back(::std::stoul(value));
  }
  ::gpiod::chip chip(argv[1]);
  auto lines = chip.get_lines(offsets);

  lines.request({argv[0], ::gpiod::line_request::DIRECTION_OUTPUT, 0}, values);
  int toggle = 1;
  for (;;) {
    toggle == 0 ? toggle = 1 : toggle = 0;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    lines.set_values({toggle});
  }

  return EXIT_SUCCESS;
}
