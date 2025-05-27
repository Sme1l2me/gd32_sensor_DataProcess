#include "gd32f10x.h"
#include "systick.h"
#include "common.h"
#include "bmp390.h"

#define Standard_atmospheric_pressure 101325.0
#define Offest_Pressure 0

void bmp390_i2c_init(void)
{
  gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_10 | GPIO_PIN_11);

	i2c_deinit(I2C1);
	
  i2c_clock_config(I2C1, 100000, I2C_DTCY_2);

  i2c_mode_addr_config(I2C1, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, BMP390_WRITE_ADDRESS);

  i2c_enable(I2C1);	
	
	i2c_ack_config(I2C1, I2C_ACK_ENABLE);
}

void bmp390_init(void)
{
  uint8_t command[2];
	
	command[0] = BMP390_PWR_CTRL;
	command[1] = 0x33;
	i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, command, sizeof(command));

//	command[0] = BMP390_OSR;
//	command[1] = 0x15;
//	i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, command, sizeof(command));
//	
//	command[0] = BMP390_ODR;
//	command[1] = 0x04;
//	i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, command, sizeof(command));
}

void bmp390_get_status(void)
{
  uint8_t command = BMP390_STATUS;
  uint8_t buffer[1];

	i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &command, sizeof(command));

  delay_1ms(5);

  i2c_start_on_bus(I2C1);
  while(!i2c_flag_get(I2C1, I2C_FLAG_SBSEND));
  i2c_master_addressing(I2C1, BMP390_WRITE_ADDRESS, I2C_RECEIVER);
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

void bmp390_read_value(uint32_t *pressure, uint32_t *temperature)
{
	uint8_t command;
  uint8_t buffer[6];
  uint32_t raw_pressure, raw_temperature;

	command = BMP390_PRESSURE_XLSB;
	i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &command, sizeof(command));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &buffer[0], 1);

	command = BMP390_PRESSURE_LSB;
	i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &command, sizeof(command));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &buffer[1], 1);
	
	command = BMP390_PRESSURE_MSB;
	i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &command, sizeof(command));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &buffer[2], 1);
	
	command = BMP390_TEMPERATURE_XLSB;
	i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &command, sizeof(command));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &buffer[3], 1);
	
	command = BMP390_TEMPERATURE_LSB;
	i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &command, sizeof(command));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &buffer[4], 1);
	
	command = BMP390_TEMPERATURE_MSB;
	i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &command, sizeof(command));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &buffer[5], 1);
	
  raw_pressure = ((uint32_t)buffer[2] << 16) | ((uint32_t)buffer[1] << 8) | (uint32_t)buffer[0];
  raw_temperature = ((uint32_t)buffer[5] << 16) | ((uint32_t)buffer[4] << 8) | (uint32_t)buffer[3];

  *temperature = raw_temperature;

  *pressure = raw_pressure;
}

void Parameter_Reading(int *Pressure_Para, int *Temperature_Para)
{
  uint8_t nvm_reg_addr;
  uint8_t temp_config0, temp_config1, temp_config2, temp_config3, temp_config4;
  uint8_t press_config0, press_config1, press_config2, press_config3, press_config4;
  uint8_t press_config5, press_config6, press_config7, press_config8, press_config9;
  uint8_t press_config10, press_config11, press_config12, press_config13, press_config14;
  uint8_t press_config15;

  //Temperature coefficients
  nvm_reg_addr = BMP390_NVM_PAR_T1_L;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &temp_config0, 1);
  nvm_reg_addr = BMP390_NVM_PAR_T1_H;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &temp_config1, 1);
  nvm_reg_addr = BMP390_NVM_PAR_T2_L;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &temp_config2, 1);
  nvm_reg_addr = BMP390_NVM_PAR_T2_H;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &temp_config3, 1);
  nvm_reg_addr = BMP390_NVM_PAR_T3;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &temp_config4, 1);

  Temperature_Para[0] = (temp_config1 << 8) + temp_config0;
  Temperature_Para[1] = (temp_config3 << 8) + temp_config2;
  Temperature_Para[2] = temp_config4;
  if(Temperature_Para[2] & 0x80)
    Temperature_Para[2] = Temperature_Para[2] - Total_Number_8;
  
  //Pressure coefficients
  nvm_reg_addr = BMP390_NVM_PAR_P1_L;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &press_config0, 1);
  nvm_reg_addr = BMP390_NVM_PAR_P1_H;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &press_config1, 1);
  nvm_reg_addr = BMP390_NVM_PAR_P2_L;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &press_config2, 1);
  nvm_reg_addr = BMP390_NVM_PAR_P2_H;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &press_config3, 1);
  nvm_reg_addr = BMP390_NVM_PAR_P3;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &press_config4, 1);
  nvm_reg_addr = BMP390_NVM_PAR_P4;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &press_config5, 1);
  nvm_reg_addr = BMP390_NVM_PAR_P5_L;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &press_config6, 1);
  nvm_reg_addr = BMP390_NVM_PAR_P5_H;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &press_config7, 1);
  nvm_reg_addr = BMP390_NVM_PAR_P6_L;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &press_config8, 1);
  nvm_reg_addr = BMP390_NVM_PAR_P6_H;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &press_config9, 1);
  nvm_reg_addr = BMP390_NVM_PAR_P7;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &press_config10, 1);
  nvm_reg_addr = BMP390_NVM_PAR_P8;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &press_config11, 1);
  nvm_reg_addr = BMP390_NVM_PAR_P9_L;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &press_config12, 1);
  nvm_reg_addr = BMP390_NVM_PAR_P9_H;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &press_config13, 1);
  nvm_reg_addr = BMP390_NVM_PAR_P10;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &press_config14, 1);
  nvm_reg_addr = BMP390_NVM_PAR_P11;
  i2c_master_transmitter(I2C1, BMP390_WRITE_ADDRESS, &nvm_reg_addr, sizeof(nvm_reg_addr));
	i2c_master_receiver(I2C1, BMP390_READ_ADDRESS, &press_config15, 1);

  //Coefficient P1
  Pressure_Para[0] = (press_config1 << 8) + press_config0;
  if(Pressure_Para[0] & 0x8000)
    Pressure_Para[0] = Pressure_Para[0] - Total_Number_16;//P1
  //Coefficient P2
  Pressure_Para[1] = (press_config3 << 8) + press_config2;
  if(Pressure_Para[1] & 0x8000)
    Pressure_Para[1] = Pressure_Para[1] - Total_Number_16;//P2
  //Coefficient P3
  Pressure_Para[2] = press_config4;
  if(Pressure_Para[2] & 0x80)
    Pressure_Para[2] = Pressure_Para[2] - Total_Number_8;//P3
  //Coefficient P4
  Pressure_Para[3] = press_config5;
  if(Pressure_Para[3] & 0x80)
    Pressure_Para[3] = Pressure_Para[3] - Total_Number_8;//P4
  //Coefficient P5
  Pressure_Para[4] = (press_config7 << 8) + press_config6;//P5
  //Coefficient P6
  Pressure_Para[5] = (press_config9 << 8) + press_config8;//P6
  //Coefficient P7
  Pressure_Para[6] = press_config10;
  if(Pressure_Para[6] & 0x80)
    Pressure_Para[6] = Pressure_Para[6] - Total_Number_8;//P7
  //Coefficient P8
  Pressure_Para[7] = press_config11;
  if(Pressure_Para[7] & 0x80)
    Pressure_Para[7] = Pressure_Para[7] - Total_Number_8;//P8
  //Coefficient P9
  Pressure_Para[8] = (press_config13 << 8) + press_config12;
  if(Pressure_Para[8] & 0x8000)
    Pressure_Para[8] = Pressure_Para[8] - Total_Number_16;//P9
  //Coefficient P10
  Pressure_Para[9] = press_config14;
  if(Pressure_Para[9] & 0x80)
    Pressure_Para[9] = Pressure_Para[9] - Total_Number_8;//P10
  //Coefficient P11
  Pressure_Para[10] = press_config15;
  if(Pressure_Para[10] & 0x80)
    Pressure_Para[10] = Pressure_Para[10] - Total_Number_8;//P11
}

double Correcting_Pressure(uint32_t Pressure, int *Pressure_Para, double Corr_Temperature)
{
  double PAR_P1, PAR_P2, PAR_P3, PAR_P4, PAR_P5;
  double PAR_P6, PAR_P7, PAR_P8, PAR_P9, PAR_P10, PAR_P11;
  double Corr_Pressure, partial_data1, partial_data2, partial_data3, partial_data4;
  double partial_out1, partial_out2;

  PAR_P1 = (Pressure_Para[0]-Total_Number_14)/Total_Number_20;
  PAR_P2 = (Pressure_Para[1]-Total_Number_14)/Total_Number_29;
  PAR_P3 = Pressure_Para[2]/Total_Number_32;
  PAR_P4 = Pressure_Para[3]/Total_Number_32/Total_Number_5;
  PAR_P5 = Pressure_Para[4]/Total_Number_Neg_3;
  PAR_P6 = Pressure_Para[5]/Total_Number_6;
  PAR_P7 = Pressure_Para[6]/Total_Number_8;
  PAR_P8 = Pressure_Para[7]/Total_Number_15;
  PAR_P9 = Pressure_Para[8]/Total_Number_32/Total_Number_16;
  PAR_P10 = Pressure_Para[9]/Total_Number_32/Total_Number_16;
  PAR_P11 = Pressure_Para[10]/Total_Number_32/Total_Number_32/Total_Number_1;

  //Calculation
  partial_data1 = PAR_P6*Corr_Temperature;
  partial_data2 = PAR_P7*Corr_Temperature*Corr_Temperature;
  partial_data3 = PAR_P8*Corr_Temperature*Corr_Temperature*Corr_Temperature;
  partial_out1 = PAR_P5+partial_data1+partial_data2+partial_data3;

  partial_data1 = PAR_P2*Corr_Temperature;
  partial_data2 = PAR_P3*Corr_Temperature*Corr_Temperature;
  partial_data3 = PAR_P4*Corr_Temperature*Corr_Temperature*Corr_Temperature;
  partial_out2 = (double)(Pressure)*(PAR_P1+partial_data1+partial_data2+partial_data3);
  
  partial_data1 = (double)(Pressure)*(double)(Pressure);
  partial_data2 = PAR_P9+PAR_P10*Corr_Temperature;
  partial_data3 = partial_data1*partial_data2;
  partial_data4 = partial_data3+(double)(Pressure)*(double)(Pressure)*(double)(Pressure)*PAR_P11;
  Corr_Pressure = partial_out1+partial_out2+partial_data4;
  return Corr_Pressure;
}

double Correcting_Temperature(uint32_t Temperature, int *Temperature_Para)
{	
	double Corr_Temperature, PAR_T1, PAR_T2, PAR_T3;
	double partial_data1, parital_data2;

	PAR_T1 = Temperature_Para[0]/Total_Number_Neg_8;
	PAR_T2 = Temperature_Para[1]/Total_Number_30;
	PAR_T3 = Temperature_Para[2]/Total_Number_32/Total_Number_16;

	//Calculation
	partial_data1 = (double)(Temperature)-PAR_T1;
	parital_data2 = partial_data1*PAR_T2;
	Corr_Temperature = parital_data2+partial_data1*partial_data1*PAR_T3;
	return Corr_Temperature;
}

void bmp390_run_once(double *Correcting_Temp, double *Correcting_Press)
{
  uint32_t pressure, temperature;
  int Pressure_Para[11], Temperature_Pera[3];

  bmp390_init();

  Parameter_Reading(Pressure_Para, Temperature_Pera);
  // bmp390_get_status();

  bmp390_read_value(&pressure, &temperature);
  *Correcting_Temp = Correcting_Temperature(temperature, Temperature_Pera);
  *Correcting_Press = Correcting_Pressure(pressure, Pressure_Para, *Correcting_Temp) + Offest_Pressure;
  BMP390_PRT("Temperature: %.2fC Pressure: %.2fPa \r\n", *Correcting_Temp, *Correcting_Press);
}
