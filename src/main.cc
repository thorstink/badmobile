#include "nlohmann/json.hpp"
#include <atomic>
#include <gpiod.hpp>
#include <iostream>
#include <seasocks/PageHandler.h>
#include <seasocks/PrintfLogger.h>
#include <seasocks/Server.h>
#include <seasocks/StringUtil.h>
#include <seasocks/WebSocket.h>
#include <thread>

using namespace seasocks;
using json = nlohmann::json;

auto chip_nr = "0";
auto gpio_nr = "14";

::std::vector<unsigned int> offsets;
::std::vector<int> values;

struct CmdVelHandler : WebSocket::Handler {
  CmdVelHandler(std::function<void()> x) : f(x){};
  std::set<WebSocket *> _cons;
  int c = 0;
  std::function<void()> f;

  void onConnect(WebSocket *con) override { _cons.insert(con); }
  void onDisconnect(WebSocket *con) override { _cons.erase(con); }

  void onData(WebSocket * /*con*/, const char *data) override {
    auto j = json::parse(data);
    std::cout << "received: " << c++ << std::endl;

    // add stuff to reply:
    json r;
    r["received"] = true;
    r["linear_x"] = j.at("linear_x");
    r["angular_z"] = j.at("angular_z");

    f();

    send(r);
  }

  void send(const json &r) {
    for (auto *con : _cons) {
      con->send(r.dump());
    }
  }
};

int main() {
  int toggle = 1;
  values.push_back(::std::stoul(gpio_nr));
  offsets.push_back(::std::stoul(gpio_nr));

  ::gpiod::chip chip(chip_nr);
  auto lines = chip.get_lines(offsets);
  lines.request({chip_nr, ::gpiod::line_request::DIRECTION_OUTPUT, 0}, values);
  auto led_iface = [&toggle, &lines] {
    toggle == 0 ? toggle = 1 : toggle = 0;
    lines.set_values({toggle});
    return;
  };
  auto cmd_vel_handle = std::make_shared<CmdVelHandler>(led_iface);

  Server server(std::make_shared<PrintfLogger>(Logger::Level::Error));

  server.addWebSocketHandler("/cmd", cmd_vel_handle);
  server.serve("ui", 2222);
  return 0;
}