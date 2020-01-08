#include "nlohmann/json.hpp"
#include <atomic>
#include <boost/fiber/all.hpp>
#include <boost/lockfree/queue.hpp>
#include <iostream>
#include <seasocks/PageHandler.h>
#include <seasocks/PrintfLogger.h>
#include <seasocks/Server.h>
#include <seasocks/StringUtil.h>
#include <seasocks/WebSocket.h>
#include <thread>

using namespace boost::lockfree;
using namespace boost::fibers;
using namespace seasocks;
using json = nlohmann::json;

struct CmdVelHandler : WebSocket::Handler {
  std::set<WebSocket *> _cons;
  void onConnect(WebSocket *con) override {
    _cons.insert(con);
    // send(con->credentials()->username + "someone has joined");
  }
  void onDisconnect(WebSocket *con) override {
    _cons.erase(con);
    // std::cout << con->credentials()->username + "has left" << std::endl;
  }

  void onData(WebSocket * /*con*/, const char *data) override {
    json j(data);
    send(j);
    std::cout << "received: " << j.dump() << std::endl;
  }

  void send(const json & /*j*/) {
    for (auto *con : _cons) {
      json j = {{"received", 1.0}};
      con->send(j.dump());
      std::cout << "send: " << j.dump() << std::endl;
    }
  }
};

queue<int, fixed_sized<true>> q{10000};
std::atomic<int> sum{0};

void produce() {
  for (int i = 1; i <= 10000; ++i)
    q.push(i);
}

void consume() {
  int i;
  while (q.pop(i))
    sum += i;
}

int main() {
  auto cmd_vel_handle = std::make_shared<CmdVelHandler>();
  Server server(std::make_shared<PrintfLogger>(Logger::Level::Error));

  boost::fibers::fiber f1{produce};
  boost::fibers::fiber f2{consume};
  boost::fibers::fiber f3{consume};
  f1.join();
  f2.join();
  f3.join();

  server.addWebSocketHandler("/cmd", cmd_vel_handle);
  server.serve("ui", 2222);
  return 0;
}