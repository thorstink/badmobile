#include <chrono>
#include <cstdlib>
#include <gpiod.hpp>
#include <iostream>
#include <thread>

// https://github.com/aquaticus/nexus433/blob/master/CMakeLists.txt

int main(int, char **) {

  auto chip_nr = "0";
  auto gpio_nr = "14";

  ::std::vector<unsigned int> offsets;
  ::std::vector<int> values;

  values.push_back(::std::stoul(gpio_nr));
  offsets.push_back(::std::stoul(gpio_nr));

  ::gpiod::chip chip(chip_nr);
  auto lines = chip.get_lines(offsets);

  lines.request({chip_nr, ::gpiod::line_request::DIRECTION_OUTPUT, 0}, values);
  int toggle = 1;
  for (;;) {
    toggle == 0 ? toggle = 1 : toggle = 0;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    lines.set_values({toggle});
  }

  return EXIT_SUCCESS;
}
