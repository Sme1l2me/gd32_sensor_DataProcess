#include "gd32f10x.h"
#include "systick.h"
#include "qmi8658a.h"
#include "common.h"

void qmi8658a_i2c_init(void)
{
  gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);

  i2c_deinit(I2C0);

  i2c_clock_config(I2C0, 100000, I2C_DTCY_2);

  i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, QMI8658A_SLAVE_ADDRESS);

  i2c_enable(I2C0);

  i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}

// void qmi8658a_softreset(void)
// {
//   uint8_t command[2] = {0x60, 0xb0};
//   uint8_t resetAddr[1] = {0x4d};
//   uint8_t resetBuffer[1];

//   //step1
//   while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
//   i2c_start_on_bus(I2C0);
//   while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));

//   i2c_master_addressing(I2C0, QMI8658A_SLAVE_ADDRESS, I2C_TRANSMITTER);
//   /* wait until ADDSEND bit is set */
//   while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
//   /* clear ADDSEND bit */
//   i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

//   for (uint8_t i = 0; i < 2; i++) {
//     while (!i2c_flag_get(I2C0, I2C_FLAG_TBE));
//     i2c_data_transmit(I2C0, command[i]);
//   }

//   while (!i2c_flag_get(I2C0, I2C_FLAG_BTC));
//   i2c_stop_on_bus(I2C0);

//   delay_1ms(15);

//   //step2
//   while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
//   i2c_start_on_bus(I2C0);
//   while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));

//   i2c_master_addressing(I2C0, QMI8658A_SLAVE_ADDRESS, I2C_TRANSMITTER);
//   /* wait until ADDSEND bit is set */
//   while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));
//   /* clear ADDSEND bit */
//   i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

//   while (!i2c_flag_get(I2C0, I2C_FLAG_TBE));
//   i2c_data_transmit(I2C0, resetAddr[0]);

//   while (!i2c_flag_get(I2C0, I2C_FLAG_BTC));
//   i2c_stop_on_bus(I2C0);
  
//   //step3
//   while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));
//   i2c_start_on_bus(I2C0);
//   i2c_master_addressing(I2C0, QMI8658A_SLAVE_ADDRESS, I2C_RECEIVER);
//   i2c_ack_config(I2C0, I2C_ACK_ENABLE);

//   /* wait until the RBNE bit is set */
//   while(!i2c_flag_get(I2C0, I2C_FLAG_RBNE));
//   resetBuffer[0] = i2c_data_receive(I2C0);

//   /* send a NACK for the last data byte */
//   i2c_ack_config(I2C0, I2C_ACK_DISABLE);
//   while(!i2c_flag_get(I2C0, I2C_FLAG_BTC));
//   i2c_stop_on_bus(I2C0);

//   if(resetBuffer[0] == 0x80) {
//     QMI8658A_PRT("QMI8658A Soft Reset success\r\n");
//   } else {
//     QMI8658A_PRT("QMI8658A Soft Reset Failed! resetAddrValue:%x\r\n", resetBuffer[0]);
//     return;
//   }
// }

void qmi8658a_configmode(void)
{
  uint8_t command[2] = {CTRL1, 0x00};

  i2c_master_transmitter(I2C0, QMI8658A_SLAVE_ADDRESS, command, sizeof(command));
}

void qmi8658a_enable(void)
{
  uint8_t command[2] = {CTRL7, 0x03};

  i2c_master_transmitter(I2C0, QMI8658A_SLAVE_ADDRESS, command, sizeof(command));
}

void qmi8658a_getstatus(void)
{
  uint8_t command = STATUS;
  uint8_t buffer = 0;

  while(1) {
    i2c_master_transmitter(I2C0, QMI8658A_SLAVE_ADDRESS, &command, sizeof(command));
		
    i2c_master_receiver(I2C0, QMI8658A_SLAVE_ADDRESS, &buffer, sizeof(buffer));
		delay_1ms(1);
    if(((buffer >> 0) & 1 ) && (((buffer >> 1) & 1) == 0)) {
      break;
    }
  }
}

//aODR 1000hz aFS ±16g
void qmi8658a_config_acceleration(void)
{
  uint8_t command[2] = {CTRL2, 0xb3};

  i2c_master_transmitter(I2C0, QMI8658A_SLAVE_ADDRESS, command, sizeof(command));
}

//gODR 1793.6hz gFS ±2048°/s
void qmi8658a_config_angularrate(void)
{
  uint8_t command[2] = {CTRL3, 0xf2};

  i2c_master_transmitter(I2C0, QMI8658A_SLAVE_ADDRESS, command, sizeof(command));
}

void qmi8658a_init(void)
{
  qmi8658a_configmode();

  qmi8658a_enable();

  qmi8658a_config_acceleration();
  qmi8658a_config_angularrate();
}

void qmi8658a_read_acceleration(float *ax, float *ay, float *az) 
{
  uint8_t command;
  uint8_t buffer[2];
  int16_t ax_raw, ay_raw, az_raw;

  command = AX_H;
  i2c_ack_config(I2C0, I2C_ACK_ENABLE);

  i2c_master_transmitter(I2C0, QMI8658A_SLAVE_ADDRESS, &command, sizeof(command));

  i2c_master_receiver(I2C0, QMI8658A_SLAVE_ADDRESS, buffer, sizeof(buffer));
	delay_1ms(10);
  ax_raw = (buffer[0] << 8) | buffer[1];
	
  command = AY_H;
  i2c_ack_config(I2C0, I2C_ACK_ENABLE);
  i2c_master_transmitter(I2C0, QMI8658A_SLAVE_ADDRESS, &command, sizeof(command));

  i2c_master_receiver(I2C0, QMI8658A_SLAVE_ADDRESS, buffer, sizeof(buffer));
	delay_1ms(10);
  ay_raw = (buffer[0] << 8) | buffer[1];
	
	
  command = AZ_H;
  i2c_ack_config(I2C0, I2C_ACK_ENABLE);
  i2c_master_transmitter(I2C0, QMI8658A_SLAVE_ADDRESS, &command, sizeof(command));

  i2c_master_receiver(I2C0, QMI8658A_SLAVE_ADDRESS, buffer, sizeof(buffer));
	delay_1ms(10);
  az_raw = (buffer[0] << 8) | buffer[1];
	
  *ax = (float)ax_raw / 2048;   //a = raw_data / (2^15 / aFS)
  *ay = (float)ay_raw / 2048; 
  *az = (float)az_raw / 2048;
}

void qmi8658a_read_angularrate(float *gx, float *gy, float *gz)
{
  uint8_t command;
  uint8_t buffer[2];
  int16_t gx_raw, gy_raw, gz_raw;

  command = GX_H;
  i2c_ack_config(I2C0, I2C_ACK_ENABLE);
  i2c_master_transmitter(I2C0, QMI8658A_SLAVE_ADDRESS, &command, sizeof(command));

  i2c_master_receiver(I2C0, QMI8658A_SLAVE_ADDRESS, buffer, sizeof(buffer));
	delay_1ms(10);
  gx_raw = (buffer[0] << 8) | buffer[1];
	
  command = GY_H;
  i2c_ack_config(I2C0, I2C_ACK_ENABLE);
  i2c_master_transmitter(I2C0, QMI8658A_SLAVE_ADDRESS, &command, sizeof(command));

  i2c_master_receiver(I2C0, QMI8658A_SLAVE_ADDRESS, buffer, sizeof(buffer));
	delay_1ms(10);
  gy_raw = (buffer[0] << 8) | buffer[1];
	
  command = GZ_H;
  i2c_ack_config(I2C0, I2C_ACK_ENABLE);
  i2c_master_transmitter(I2C0, QMI8658A_SLAVE_ADDRESS, &command, sizeof(command));
  
  i2c_master_receiver(I2C0, QMI8658A_SLAVE_ADDRESS, buffer, sizeof(buffer));
	delay_1ms(10);
  gz_raw = (buffer[0] << 8) | buffer[1];

  *gx = (float)gx_raw / 16;   //g = raw_data / (2^15 / gFS)
  *gy = (float)gy_raw / 16;
  *gz = (float)gz_raw / 16;
}

void qmi8658a_run_once(float *ax, float *ay, float *az, float *gx, float *gy ,float *gz)
{
  qmi8658a_getstatus();
  qmi8658a_read_acceleration(ax, ay, az);
  qmi8658a_read_angularrate(gx, gy, gz);
  QMI8658A_PRT("Acceleration: AX=%.2fg, AY=%.2fg, AZ=%.2fg\r\n", *ax, *ay, *az);
  QMI8658A_PRT("AngularRate: GX=%.2f/s, GY=%.2f/s, GZ=%.2f/s\r\n", *gx, *gy, *gz);
}
