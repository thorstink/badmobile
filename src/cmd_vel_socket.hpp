#pragma once

#include "nlohmann/json.hpp"
#include <functional>
#include <iostream>
#include <seasocks/PageHandler.h>
#include <seasocks/PrintfLogger.h>
#include <seasocks/Server.h>
#include <seasocks/StringUtil.h>
#include <seasocks/WebSocket.h>

struct CmdVelHandler : seasocks::WebSocket::Handler {
  CmdVelHandler(std::function<void()> x) : f(x){};
  std::set<seasocks::WebSocket *> _cons;
  std::function<void()> f;

  void onConnect(seasocks::WebSocket *con) override { _cons.insert(con); }
  void onDisconnect(seasocks::WebSocket *con) override { _cons.erase(con); }

  void onData(seasocks::WebSocket * /*con*/, const char *data) override {
    auto j = nlohmann::json::parse(data);
    std::cout << "received " << std::endl;

    // add stuff to reply:
    nlohmann::json r;
    r["received"] = true;
    r["linear_x"] = j.at("linear_x");
    r["angular_z"] = j.at("angular_z");

    f();

    send(r);
  }

  void send(const nlohmann::json &r) const {
    for (auto *con : _cons) {
      con->send(r.dump());
    }
  }
};
