extern "C" {
#include <pigpio.h>
#include <stdio.h>
}

#include "lsm9ds1/lsm9ds1.h"
#include <chrono>
#include <iostream>
#include <math.h> /* sin */
#include <thread>

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

int main() {

  if (gpioInitialise() < 0)
    std::exception_ptr();

  unsigned SPI_CHANNEL = 0;
  unsigned SPI_BAUD = 5000000;
  unsigned SPI_FLAGS = 0;

  int g_spi_handle, r, e, b;

  gpioSetMode(CSAG, PI_OUTPUT);

  g_spi_handle = spiOpen(SPI_CHANNEL, SPI_BAUD, SPI_FLAGS);
  printf("open handler code %d\n", g_spi_handle);

  const auto read_ag_u8 = [&g_spi_handle](char address) {
    uint8_t tx = SPI_READ | address;
    uint8_t rx;
    gpioWrite(CSAG, 0);
    spiWrite(g_spi_handle, (char *)(&tx), 1);
    spiRead(g_spi_handle, (char *)(&rx), 1);
    gpioWrite(CSAG, 1);
    return rx;
  };

  const auto write_ag_u8 = [&g_spi_handle](char address, char value) {
    uint8_t tx1 = SPI_WRITE | address;
    uint8_t tx2 = value;
    gpioWrite(CSAG, 0);
    spiWrite(g_spi_handle, (char *)(&tx1), 1);
    spiWrite(g_spi_handle, (char *)(&tx2), 1);
    gpioWrite(CSAG, 1);
    return;
  };

  const auto readXYZ = [&g_spi_handle](char start_address) {
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

  gpioWrite(CSAG, 0);

  // soft reset & reboot accel/gyro
  write_ag_u8(LSM9DS1_REGISTER_CTRL_REG8, 0x05);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  // enable gyro continuous
  write_ag_u8(LSM9DS1_REGISTER_CTRL_REG1_G, LSM9DS1_REGISTER_CTRL_REG1_G_ON);
  // Enable the accelerometer continous
  write_ag_u8(LSM9DS1_REGISTER_CTRL_REG5_XL, 0x38);
  // 1 KHz out data rate, BW set by ODR, 408Hz anti-aliasing
  write_ag_u8(LSM9DS1_REGISTER_CTRL_REG6_XL, 0xC0);

  {
    // set range 2G
    uint8_t reg = read_ag_u8(LSM9DS1_REGISTER_CTRL_REG6_XL);
    reg &= ~(0b00011000);
    reg |= LSM9DS1_ACCELRANGE_2G;
    write_ag_u8(LSM9DS1_REGISTER_CTRL_REG6_XL, reg);
  }
  {
    // set dpi 245
    uint8_t reg = read_ag_u8(LSM9DS1_REGISTER_CTRL_REG1_G);
    reg &= ~(0b00110000);
    reg |= LSM9DS1_ACCELRANGE_2G;
    write_ag_u8(LSM9DS1_REGISTER_CTRL_REG1_G, reg);
  }

  printf("%x,", read_ag_u8(LSM9DS1_REGISTER_WHO_AM_I_XG));
  gpioWrite(CSAG, 1);

  for (int i = 0; i < 10000; i++) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    auto acc = readXYZ(LSM9DS1_REGISTER_OUT_X_L_XL);
    auto gyr = readXYZ(LSM9DS1_REGISTER_OUT_X_L_G);
    printf("\nacc: ");
    printf("%d,", acc.x);
    printf("%d,", acc.y);
    printf("%d.", acc.z);

    printf(" gyr: ");
    printf("%d,", gyr.x);
    printf("%d,", gyr.y);
    printf("%d,", gyr.z);
  }

  e = spiClose(g_spi_handle);

  if (e != 0) {
    printf("\nspi clode error is %d\n", e);
  } else {
    printf("\nspi closed succesfully\n");
  }

  gpioTerminate();
}
