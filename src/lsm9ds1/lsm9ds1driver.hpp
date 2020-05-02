#ifndef LSM9DS1DRIVER_HPP
#define LSM9DS1DRIVER_HPP

#include "imu_msg.hpp"
#include "lsm9ds1.h"
#include <pigpio.h>
#include <rxcpp/rx.hpp>

// "continious mode"
#define LSM9DS1_REGISTER_CTRL_REG1_G_ON 0xC0

#define SCLK 11 // SCL
#define MOSI 10 // SDA
#define MISO 9  // SDOAG
#define CSAG 5  // CSAG

struct xyz {
  int16_t x;
  int16_t y;
  int16_t z;
};

const auto read_ag_u8 = [](int g_spi_handle, char address) {
  uint8_t tx = SPI_READ | address;
  uint8_t rx;
  gpioWrite(CSAG, 0);
  spiWrite(g_spi_handle, (char *)(&tx), 1);
  spiRead(g_spi_handle, (char *)(&rx), 1);
  gpioWrite(CSAG, 1);
  return rx;
};

const auto write_ag_u8 = [](int g_spi_handle, char address, char value) {
  uint8_t tx1 = SPI_WRITE | address;
  uint8_t tx2 = value;
  gpioWrite(CSAG, 0);
  spiWrite(g_spi_handle, (char *)(&tx1), 1);
  spiWrite(g_spi_handle, (char *)(&tx2), 1);
  gpioWrite(CSAG, 1);
  return;
};

const auto readXYZ = [](int g_spi_handle, char start_address) {
  char buffer[6];
  uint8_t tx = SPI_READ | start_address;

  gpioWrite(CSAG, 0);
  spiWrite(g_spi_handle, (char *)(&tx), 1);
  spiRead(g_spi_handle, buffer, 6);
  gpioWrite(CSAG, 1);

  uint8_t xlo = buffer[0];
  int16_t xhi = buffer[1];
  uint8_t ylo = buffer[2];
  int16_t yhi = buffer[3];
  uint8_t zlo = buffer[4];
  int16_t zhi = buffer[5];

  // Shift values to create properly formed integer (low byte first)
  xhi <<= 8;
  xhi |= xlo;
  yhi <<= 8;
  yhi |= ylo;
  zhi <<= 8;
  zhi |= zlo;

  return xyz{xhi, yhi, zhi};
};

const auto init_lsm9ds1 = [](int g_spi_handle) {
  // soft reset & reboot accel/gyro
  write_ag_u8(g_spi_handle, LSM9DS1_REGISTER_CTRL_REG8, 0x05);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  // enable gyro continuous
  write_ag_u8(g_spi_handle, LSM9DS1_REGISTER_CTRL_REG1_G,
              LSM9DS1_REGISTER_CTRL_REG1_G_ON);
  // Enable the accelerometer continous
  write_ag_u8(g_spi_handle, LSM9DS1_REGISTER_CTRL_REG5_XL, 0x38);
  // 1 KHz out data rate, BW set by ODR, 408Hz anti-aliasing
  write_ag_u8(g_spi_handle, LSM9DS1_REGISTER_CTRL_REG6_XL, 0xC0);

  {
    // set range 2G
    uint8_t reg = read_ag_u8(g_spi_handle, LSM9DS1_REGISTER_CTRL_REG6_XL);
    reg &= ~(0b00011000);
    reg |= LSM9DS1_ACCELRANGE_2G;
    write_ag_u8(g_spi_handle, LSM9DS1_REGISTER_CTRL_REG6_XL, reg);
  }
  {
    // set dpi 245
    uint8_t reg = read_ag_u8(g_spi_handle, LSM9DS1_REGISTER_CTRL_REG1_G);
    reg &= ~(0b00110000);
    reg |= LSM9DS1_ACCELRANGE_2G;
    write_ag_u8(g_spi_handle, LSM9DS1_REGISTER_CTRL_REG1_G, reg);
  }

  gpioWrite(CSAG, 1);
};

inline rxcpp::observable<imu_t> createLSM9DS1Observable() {
  return rxcpp::observable<>::create<imu_t>([](rxcpp::subscriber<imu_t> s) {
    // open spi
    unsigned SPI_CHANNEL = 0;
    unsigned SPI_BAUD = 5000000;
    unsigned SPI_FLAGS = 0;
    int g_spi_handle, r, e, b;
    gpioSetMode(CSAG, PI_OUTPUT);
    g_spi_handle = spiOpen(SPI_CHANNEL, SPI_BAUD, SPI_FLAGS);
    if (g_spi_handle < 0) {
      s.on_error(std::exception_ptr()); // spi bus error
    }

    // init
    init_lsm9ds1(g_spi_handle);
    // check
    if (read_ag_u8(g_spi_handle, LSM9DS1_REGISTER_WHO_AM_I_XG) !=
        WHO_AM_I_AG_RSP) {
      s.on_error(std::exception_ptr()); // imu error
    };

    // driver setup
    std::chrono::time_point<std::chrono::steady_clock> now;
    std::chrono::time_point<std::chrono::steady_clock> wakeup_time;
    constexpr auto delta = std::chrono::milliseconds(100);

    struct xyz acc;
    struct xyz gyr;

    for (;;) {
      if (!s.is_subscribed())
        break;

      now = std::chrono::steady_clock::now();
      const auto acc = readXYZ(g_spi_handle, LSM9DS1_REGISTER_OUT_X_L_XL);
      const auto gyr = readXYZ(g_spi_handle, LSM9DS1_REGISTER_OUT_X_L_G);

      s.on_next(imu_t{now.time_since_epoch().count(), double(acc.x),
                      double(acc.y), double(acc.z), double(gyr.x),
                      double(gyr.y), double(gyr.z)});

      wakeup_time = now + delta;
      if (wakeup_time > std::chrono::steady_clock::now()) {
        std::this_thread::sleep_until(wakeup_time);
      }
    }
    e = spiClose(g_spi_handle);
    if (e != 0) {
      s.on_error(std::exception_ptr()); // spi close error
    } else {
      s.on_completed();
    }
  });
};

#endif