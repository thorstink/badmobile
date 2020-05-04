#pragma once

#include "nlohmann/json.hpp"
#include <functional>
#include <iostream>
#include <seasocks/PageHandler.h>
#include <seasocks/PrintfLogger.h>
#include <seasocks/Server.h>
#include <seasocks/StringUtil.h>
#include <seasocks/WebSocket.h>

struct SettingsHandler : seasocks::WebSocket::Handler {
  SettingsHandler(std::function<void(nlohmann::json)> handle)
      : propagate_update(handle){};
  std::set<seasocks::WebSocket *> _cons;
  std::function<void(nlohmann::json)> propagate_update;

  void onConnect(seasocks::WebSocket *con) override { _cons.insert(con); }
  void onDisconnect(seasocks::WebSocket *con) override { _cons.erase(con); }

  void onData(seasocks::WebSocket * /*con*/, const char *data) override {
    propagate_update(nlohmann::json::parse(data));
  }

  void send(const nlohmann::json &r) const {
    for (auto *con : _cons) {
      con->send(r.dump());
    }
  }
};
