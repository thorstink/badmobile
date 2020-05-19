#pragma once

// See also LSM9DS1 Register Map and Descriptions,
// http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00103319.pdf
//
// Accelerometer and Gyroscope registers
#define LSM9DS1XG_ACT_THS 0x04
#define LSM9DS1XG_ACT_DUR 0x05
#define LSM9DS1XG_INT_GEN_CFG_XL 0x06
#define LSM9DS1XG_INT_GEN_THS_X_XL 0x07
#define LSM9DS1XG_INT_GEN_THS_Y_XL 0x08
#define LSM9DS1XG_INT_GEN_THS_Z_XL 0x09
#define LSM9DS1XG_INT_GEN_DUR_XL 0x0A
#define LSM9DS1XG_REFERENCE_G 0x0B
#define LSM9DS1XG_INT1_CTRL 0x0C
#define LSM9DS1XG_INT2_CTRL 0x0D
#define LSM9DS1XG_WHO_AM_I 0x0F // should return 0x68
#define LSM9DS1XG_CTRL_REG1_G 0x10
#define LSM9DS1XG_CTRL_REG2_G 0x11
#define LSM9DS1XG_CTRL_REG3_G 0x12
#define LSM9DS1XG_ORIENT_CFG_G 0x13
#define LSM9DS1XG_INT_GEN_SRC_G 0x14
#define LSM9DS1XG_OUT_TEMP_L 0x15
#define LSM9DS1XG_OUT_TEMP_H 0x16
#define LSM9DS1XG_STATUS_REG 0x17
#define LSM9DS1XG_OUT_X_L_G 0x18
#define LSM9DS1XG_OUT_X_H_G 0x19
#define LSM9DS1XG_OUT_Y_L_G 0x1A
#define LSM9DS1XG_OUT_Y_H_G 0x1B
#define LSM9DS1XG_OUT_Z_L_G 0x1C
#define LSM9DS1XG_OUT_Z_H_G 0x1D
#define LSM9DS1XG_CTRL_REG4 0x1E
#define LSM9DS1XG_CTRL_REG5_XL 0x1F
#define LSM9DS1XG_CTRL_REG6_XL 0x20
#define LSM9DS1XG_CTRL_REG7_XL 0x21
#define LSM9DS1XG_CTRL_REG8 0x22
#define LSM9DS1XG_CTRL_REG9 0x23
#define LSM9DS1XG_CTRL_REG10 0x24
#define LSM9DS1XG_INT_GEN_SRC_XL 0x26
//#define LSM9DS1XG_STATUS_REG        0x27 // duplicate of 0x17!
#define LSM9DS1XG_OUT_X_L_XL 0x28
#define LSM9DS1XG_OUT_X_H_XL 0x29
#define LSM9DS1XG_OUT_Y_L_XL 0x2A
#define LSM9DS1XG_OUT_Y_H_XL 0x2B
#define LSM9DS1XG_OUT_Z_L_XL 0x2C
#define LSM9DS1XG_OUT_Z_H_XL 0x2D
#define LSM9DS1XG_FIFO_CTRL 0x2E
#define LSM9DS1XG_FIFO_SRC 0x2F
#define LSM9DS1XG_INT_GEN_CFG_G 0x30
#define LSM9DS1XG_INT_GEN_THS_XH_G 0x31
#define LSM9DS1XG_INT_GEN_THS_XL_G 0x32
#define LSM9DS1XG_INT_GEN_THS_YH_G 0x33
#define LSM9DS1XG_INT_GEN_THS_YL_G 0x34
#define LSM9DS1XG_INT_GEN_THS_ZH_G 0x35
#define LSM9DS1XG_INT_GEN_THS_ZL_G 0x36
#define LSM9DS1XG_INT_GEN_DUR_G 0x37
//
// Magnetometer registers
#define LSM9DS1M_OFFSET_X_REG_L_M 0x05
#define LSM9DS1M_OFFSET_X_REG_H_M 0x06
#define LSM9DS1M_OFFSET_Y_REG_L_M 0x07
#define LSM9DS1M_OFFSET_Y_REG_H_M 0x08
#define LSM9DS1M_OFFSET_Z_REG_L_M 0x09
#define LSM9DS1M_OFFSET_Z_REG_H_M 0x0A
#define LSM9DS1M_WHO_AM_I 0x0F // should be 0x3D
#define LSM9DS1M_CTRL_REG1_M 0x20
#define LSM9DS1M_CTRL_REG2_M 0x21
#define LSM9DS1M_CTRL_REG3_M 0x22
#define LSM9DS1M_CTRL_REG4_M 0x23
#define LSM9DS1M_CTRL_REG5_M 0x24
#define LSM9DS1M_STATUS_REG_M 0x27
#define LSM9DS1M_OUT_X_L_M 0x28
#define LSM9DS1M_OUT_X_H_M 0x29
#define LSM9DS1M_OUT_Y_L_M 0x2A
#define LSM9DS1M_OUT_Y_H_M 0x2B
#define LSM9DS1M_OUT_Z_L_M 0x2C
#define LSM9DS1M_OUT_Z_H_M 0x2D
#define LSM9DS1M_INT_CFG_M 0x30
#define LSM9DS1M_INT_SRC_M 0x31
#define LSM9DS1M_INT_THS_L_M 0x32
#define LSM9DS1M_INT_THS_H_M 0x33
// Set initial input parameters
enum Ascale { // set of allowable accel full scale settings
  AFS_2G = 0,
  AFS_16G,
  AFS_4G,
  AFS_8G
};

enum Aodr { // set of allowable gyro sample rates
  AODR_PowerDown = 0,
  AODR_10Hz,
  AODR_50Hz,
  AODR_119Hz,
  AODR_238Hz,
  AODR_476Hz,
  AODR_952Hz
};

enum Abw { // set of allowable accewl bandwidths
  ABW_408Hz = 0,
  ABW_211Hz,
  ABW_105Hz,
  ABW_50Hz
};

enum Gscale { // set of allowable gyro full scale settings
  GFS_245DPS = 0,
  GFS_500DPS,
  GFS_NoOp,
  GFS_2000DPS
};

enum Godr { // set of allowable gyro sample rates
  GODR_PowerDown = 0,
  GODR_14_9Hz,
  GODR_59_5Hz,
  GODR_119Hz,
  GODR_238Hz,
  GODR_476Hz,
  GODR_952Hz
};

enum Gbw {     // set of allowable gyro data bandwidths
  GBW_low = 0, // 14 Hz at Godr = 238 Hz,  33 Hz at Godr = 952 Hz
  GBW_med,     // 29 Hz at Godr = 238 Hz,  40 Hz at Godr = 952 Hz
  GBW_high,    // 63 Hz at Godr = 238 Hz,  58 Hz at Godr = 952 Hz
  GBW_highest  // 78 Hz at Godr = 238 Hz, 100 Hz at Godr = 952 Hz
};