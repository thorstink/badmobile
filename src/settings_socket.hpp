#pragma once

#include "nlohmann/json.hpp"
#include <fmt/format.h>
#include <functional>
#include <iostream>
#include <seasocks/PageHandler.h>
#include <seasocks/PrintfLogger.h>
#include <seasocks/Server.h>
#include <seasocks/StringUtil.h>
#include <seasocks/WebSocket.h>

struct SettingsHandler : seasocks::WebSocket::Handler {
  SettingsHandler(std::function<void(seasocks::WebSocket *)> on_connection,
                  std::function<void(nlohmann::json)> handle)
      : on_new_connection(on_connection), propagate_update(handle){};

  SettingsHandler(const SettingsHandler &) = delete;

  std::set<seasocks::WebSocket *> _cons;
  std::function<void(seasocks::WebSocket *)> on_new_connection;
  std::function<void(nlohmann::json)> propagate_update;

  void onConnect(seasocks::WebSocket *con) override {
    fmt::print("Connected a settings websocket connection\n");
    std::cout.flush();
    _cons.insert(con);
    on_new_connection(con);
  }
  void onDisconnect(seasocks::WebSocket *con) override {
    fmt::print("Disconnected a settings websocket connection\n");
    // ...Do..nothing? because subject?
    std::cout.flush();
    _cons.erase(con);
  }

  void onData(seasocks::WebSocket * /*con*/, const char *data) override {
    fmt::print("Receiving settings from user\n");
    std::cout.flush();
    propagate_update(nlohmann::json::parse(data));
  }

  void send(const nlohmann::json &r) const {
    fmt::print("Forwarding settings data to users\n");
    std::cout.flush();
    for (auto *con : _cons) {
      con->send(r.dump());
    }
  }
};
