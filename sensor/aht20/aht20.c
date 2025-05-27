#include "gd32f10x.h"
#include "systick.h"
#include "common.h"
#include "aht20.h"

unsigned char Calc_CRC8(unsigned char *message, unsigned char length) {
  unsigned char i;
  unsigned char byte;
  unsigned char crc = 0xFF;
  for (byte = 0; byte < length; byte++) {
    crc ^= (message[byte]);
    for (i = 8; i > 0; --i) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x31;
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

void aht20_i2c_init(void)
{
  gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_10 | GPIO_PIN_11);

	i2c_deinit(I2C1);
	
  i2c_clock_config(I2C1, 100000, I2C_DTCY_2);

  i2c_mode_addr_config(I2C1, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, AHT20_SLAVE_ADDRESS);

  i2c_enable(I2C1);

  i2c_ack_config(I2C1, I2C_ACK_ENABLE);
}

void aht20_start_measure(void)
{
  uint8_t command[4] = {0x70, 0xAC, 0x33, 0x00};
  
	i2c_ack_config(I2C1, I2C_ACK_ENABLE);
	
  i2c_master_transmitter(I2C1, AHT20_SLAVE_ADDRESS, command, sizeof(command));
	delay_1ms(80);
}

void aht20_read_value(float *humidity, float *temperature)
{
  uint8_t command = 0x71;
  uint8_t buffer[7];
  uint32_t raw_humidity, raw_temperature;
  // uint8_t crc_humidity, crc_temperature;
  
  i2c_master_transmitter(I2C1, AHT20_SLAVE_ADDRESS, &command, sizeof(command));
	
  i2c_master_receiver(I2C1, AHT20_SLAVE_ADDRESS, buffer, sizeof(buffer));
  
  // crc_humidity = Calc_CRC8(buffer, 5);  // 湿度数据CRC校验（前5个字节）
  // crc_temperature = Calc_CRC8(buffer + 3, 3);  // 温度数据CRC校验（后3个字节）

  // 将数据解析为湿度和温度值
  raw_humidity = ((uint32_t)buffer[1] << 12) | ((uint32_t)buffer[2] << 4) | ((uint32_t)buffer[3] >> 4);
  raw_temperature = ((uint32_t)(buffer[3] & 0x0F) << 16) | ((uint32_t)buffer[4] << 8) | buffer[5];

  *humidity = (float)raw_humidity * 100.0 / 0x100000;
  *temperature = (float)(raw_temperature * 200.0 / 0x100000) - 50.0;
}

void aht20_run_once(float *humidity, float *temperature)
{
  aht20_start_measure();

  aht20_read_value(humidity, temperature);
  AHT20_PRT("Humidity: %.2f%% Temperature: %.2fC\r\n", *humidity, *temperature);
}
