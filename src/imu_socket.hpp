#pragma once

#include "bno055/bno055driver.hpp"
#include "nlohmann/json.hpp"
#include <rxcpp/rx.hpp>
#include <seasocks/PageHandler.h>
#include <seasocks/PrintfLogger.h>
#include <seasocks/Server.h>
#include <seasocks/StringUtil.h>
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

// fake imu

void to_json(nlohmann::json &j, const imu_t &p) {
  j = nlohmann::json{{"t", p.t},
                     {"ax", p.linear_acceleration_x},
                     {"ay", p.linear_acceleration_y},
                     {"az", p.linear_acceleration_z},
                     {"gx", p.angular_velocity_x},
                     {"gy", p.angular_velocity_y},
                     {"gz", p.angular_velocity_z}};
}

auto createFakeImu() {
  return rxcpp::observable<>::interval(std::chrono::milliseconds(100))
      .map([](int i) {
        int64_t t = uint64_t(i) * 1e8;
        double t_d = double(t);
        double j = i;
        return imu_t{t,      sin(j),       sin(j + 0.2), sin(j + 0.4),
                     cos(j), cos(j + 0.2), cos(j + 0.4)};
      })
      .map([](const imu_t &msg) { return nlohmann::json(msg); });
};