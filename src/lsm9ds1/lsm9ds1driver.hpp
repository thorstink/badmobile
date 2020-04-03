#pragma once

extern "C" {
#include <pigpio.h>
#include <stdio.h>
}

#include "imu_msg.hpp"
#include <rxcpp/rx.hpp>
#include "lsm9ds1.h"
#define SPI_READ 0x80
#define SPI_WRITE 0x0
#define WHO_AM_I_XG         0x0F
#define WHO_AM_I_AG_RSP     0x68

namespace lsm9ds1 {

inline rxcpp::observable<imu_t> createImuObservable() {
  return rxcpp::observable<>::create<imu_t>([](rxcpp::subscriber<imu_t> s) {
    if (gpioInitialise() < 0)
       s.on_error(std::exception_ptr());

    // #define SPICS 22	//Pin 22 = BCM17
    // MOSI 0, Pin 19 = BCM 10
    // MOSI - for data from the Pi to the slave
    // MISO 0, Pin 21, BCM 9
    // MISO - for data from the slave to the Pi
    // SCLK 0, Pin 23, BCM11
    // SCLK - for a clock

    // @param xgcs SPI CS pin for accelerometer/gyro subchip
    #define csxg 23	
    #define mosi 19
    #define miso 0  
    #define CSM_AG 25	
    unsigned SPI_CHANNEL = 0; 
    unsigned SPI_BAUD = 15000000;
    unsigned SPI_FLAGS = 0;

    int g_spi_handle, r, e, b;
    gpioSetMode(CSM_AG, PI_OUTPUT);	
    gpioWrite(CSM_AG, 1);

    g_spi_handle = spiOpen(SPI_CHANNEL, SPI_BAUD, SPI_FLAGS);
    printf("open handler code %d\n", g_spi_handle);

    // uint8_t txBuf = (SPI_READ | (WHO_AM_I_XG & 0x3F));
    // char buf[2] = {(SPI_READ | (WHO_AM_I_XG ))};
    char txBuf[2] = {(SPI_READ | (WHO_AM_I_XG & 0x3F))};
    char rxBuf[2] = {0};
    // uint8_t rxBuf = 0;
      // for (const char c : rxBuf)
      //   printf("%x,",c);
    for (int i = 0; i < 5; i++)
    {
      gpioWrite(CSM_AG, 0);
      // spiXfer(g_spi_handle, (char*)(&txBuf), (char*)(&rxBuf), 2);
      spiXfer(g_spi_handle, txBuf, rxBuf, 2);
      // spiRead(g_spi_handle,buf,2);
      gpioWrite(CSM_AG, 1);

      printf("\nrx: ");
      for (const char c : rxBuf)
        printf("%x,",c);
      // for (const char c : buf)
      //   printf("%x,",c);
      // printf("%u,",rxBuf);
      // printf("%x,",buf);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    e = spiClose(g_spi_handle);

    if (e!=0)
    {
      printf("\nspi clode error is %d\n", e);
    }
    else
    {
      printf("\nspi closed succesfully\n");
    }

    gpioTerminate();
    s.on_completed();
  });
};
}
