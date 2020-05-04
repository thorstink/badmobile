#pragma once

#include "imu_msg.hpp"
#include "nlohmann/json.hpp"
#include <rxcpp/rx.hpp>
#include <seasocks/WebSocket.h>

struct ImuHandler : seasocks::WebSocket::Handler {
  std::set<seasocks::WebSocket *> _cons;
  void onConnect(seasocks::WebSocket *con) override { _cons.insert(con); }
  void onDisconnect(seasocks::WebSocket *con) override { _cons.erase(con); }

  void send(const nlohmann::json &r) const {
    for (auto *con : _cons) {
      con->send(r.dump());
    }
  }
};

rxcpp::observable<imu_t> createFakeImu(const nlohmann::json &imu_settings) {
  const int fs = imu_settings["imu_sampling_frequency"];
  return rxcpp::observable<>::interval(std::chrono::microseconds(1000000 / fs))
      .map([](int i) {
        int64_t t = uint64_t(i) * 1e8;
        double t_d = double(t);
        double j = i;
        return imu_t{t,      sin(j),       sin(j + 0.2), sin(j + 0.4),
                     cos(j), cos(j + 0.2), cos(j + 0.4)};
      });
};
