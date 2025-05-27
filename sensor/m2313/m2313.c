#include "gd32f10x.h"
#include "systick.h"
#include "common.h"
#include "m2313.h"

void m2313_i2c_init(void)
{
  gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_10 | GPIO_PIN_11);

	i2c_deinit(I2C1);
	
  i2c_clock_config(I2C1, 100000, I2C_DTCY_2);

  i2c_mode_addr_config(I2C1, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, M2313_SLAVE_ADDRESS);

  i2c_enable(I2C1);	
	
	i2c_ack_config(I2C1, I2C_ACK_ENABLE);
}

void m2313_get_cal(void)
{
  uint8_t command[2] = {0x78, 0xAC};

	i2c_master_transmitter(I2C1, M2313_SLAVE_ADDRESS, command, sizeof(command));
}

void m2313_get_status(void)
{
  uint8_t command = 0x79;
  uint8_t buffer[1];

	i2c_master_transmitter(I2C1, M2313_SLAVE_ADDRESS, &command, sizeof(command));

  delay_1ms(5);

  i2c_start_on_bus(I2C1);
  while(!i2c_flag_get(I2C1, I2C_FLAG_SBSEND));
  i2c_master_addressing(I2C1, M2313_SLAVE_ADDRESS, I2C_RECEIVER);
  /* wait until ADDSEND bit is set */
  while(!i2c_flag_get(I2C1, I2C_FLAG_ADDSEND));
  /* clear ADDSEND bit */
  i2c_flag_clear(I2C1, I2C_FLAG_ADDSEND);

  while(1) {
    while(!i2c_flag_get(I2C1, I2C_FLAG_RBNE));
    buffer[0] = i2c_data_receive(I2C1);
    if(((buffer[0] >> 5) & 1 ) == 0) {
      break;
    }
  }

  while(!i2c_flag_get(I2C1, I2C_FLAG_BTC));
  i2c_stop_on_bus(I2C1);
}

void m2313_read_value(float *pressure, float *temperature)
{
  uint8_t command = 0x79;
  uint8_t buffer[6];
  uint32_t raw_bridge, raw_temperature;
  float bridge;

	i2c_master_transmitter(I2C1, M2313_SLAVE_ADDRESS, &command, sizeof(command));

	i2c_master_receiver(I2C1, M2313_SLAVE_ADDRESS, buffer, sizeof(buffer));

  raw_bridge = ((uint32_t)buffer[1] << 16) | ((uint32_t)buffer[2] << 8) | (uint32_t)buffer[3];
  raw_temperature = ((uint16_t)buffer[4] << 8) | (uint16_t)buffer[5];

  *temperature = (float)(raw_temperature * 190 / 0x10000) - 40;

  bridge = (float)raw_bridge * 100 / 0x1000000;
  *pressure = (bridge - (0x1000000 * 0.1)) * (1100 / (0.8 * 0x1000000));     //Pmax:1100 Pmin:0
}

void m2313_run_once(void)
{
  float pressure, temperature;

  m2313_get_cal();

  // m2313_get_status();

  m2313_read_value(&pressure, &temperature);
  M2313_PRT("Temperature: %.2fC Pressure: %.2fhPa \r\n",temperature, pressure);
}
