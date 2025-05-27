#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include "gd32f10x.h"

#include "aht20.h"
#include "gm702b.h"
#include "jxo2101.h"
#include "lxdgb5a1dphy.h"
#include "m2313.h"
#include "mhz19d.h"
#include "cx1088.h"
#include "msm261s4030h0r.h"
#include "pms9103s.h"
#include "qmi8658a.h"
#include "crd1503g.h"
#include "e_r_sensor.h"
#include "bmp390.h"

#define FRAME_HEADER1 0x66
#define FRAME_HEADER2 0x33
#define FRAME_TAIL1   0x33
#define FRAME_TAIL2   0x66

typedef struct Sensor_Data_Type
{
  struct {
    float humidity;             //温湿度传感器,湿度
    float temperature;          //温湿度传感器,温度
  } aht20_h_t_data;
  struct {
    float ax;                   //六轴运动传感器,加速度x轴
    float ay;                   //六轴运动传感器,加速度y轴
    float az;                   //六轴运动传感器,加速度z轴
    float gx;                   //六轴运动传感器,角速度x轴
    float gy;                   //六轴运动传感器,角速度y轴
    float gz;                   //六轴运动传感器,角速度z轴
  } qmi8658a_a_g_data;
  struct {
    double temperature;         //大气压传感器,温度
    double pressure;            //大气压传感器,压力
  } bmp390_t_p_data;
  struct {
    uint16_t pm1_0;             //颗粒传感器,pm1.0
    uint16_t pm2_5;             //颗粒传感器,pm2.5
    uint16_t pm10;              //颗粒传感器,pm10
    uint8_t reserved[2];        //2字节填充
  } pms9103s_pm_data;
  int mhz19d_co2_ppm;           //二氧化碳传感器,co2
  float jxo2101_o2_ppm;         //氧气传感器,o2
  int msm261s4030h0r_audio_value;       //噪音传感器
  float gm702b_ppm;             //一氧化碳传感器,co
  float lxdgb5a1dphy_lux;       //光照传感器
  float cx1088_gas;             //烟雾传感器
  float ersensor_value;         //电磁辐射传感器
  uint32_t crd1503g_pulse;      //辐射传感器,脉冲
} Sensor_D_T; //传感器数据结构体,总共88字节

typedef struct {
  uint8_t head[2];              //帧头
  uint8_t reserved1[6];          //6字节填充
  Sensor_D_T sensor_data;       //传感器数据
  uint16_t crc;                 //校验
  uint8_t tail[2];              //帧尾
  uint8_t reserved2[4];          //4字节填充
} Sensor_Frame_T;

void sensor_periph_clk_enable(void);
void sensor_power_enable(void);
void sensor_init(void);
void uart_send_data(uint32_t uartx, uint8_t *data, uint8_t length);
void uart_receive_data(uint32_t uartx, uint8_t *data, uint8_t length);
void i2c_master_transmitter(uint32_t i2c_periph, uint32_t i2c_addr, uint8_t *data, uint8_t length);
void i2c_master_receiver(uint32_t i2c_periph, uint32_t i2c_addr, uint8_t *data, uint8_t length);
void i2c_scan_bus(bool scan_10bit);
#endif
