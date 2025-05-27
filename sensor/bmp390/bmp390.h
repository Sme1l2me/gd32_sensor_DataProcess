#ifndef _bmp390_H_
#define _bmp390_H_

#include <stdio.h>

#define BMP390_WRITE_ADDRESS   		0xEC
#define BMP390_READ_ADDRESS				0xED

/* reg map */
#define BMP390_ERR_REG            0x02
#define BMP390_STATUS             0x03

#define BMP390_PRESSURE_XLSB      0x04
#define BMP390_PRESSURE_LSB       0x05
#define BMP390_PRESSURE_MSB       0x06

#define BMP390_TEMPERATURE_XLSB   0x07
#define BMP390_TEMPERATURE_LSB    0x08
#define BMP390_TEMPERATURE_MSB    0x09

#define BMP390_SOFT_RESET         0x10       //bit[0]: "1"after device power up or softreset
#define BMP390_IF_CONF            0x1A       //config spi mode or enable i2c watchdog
#define BMP390_PWR_CTRL           0x1B       //enable pressure and temperature sensor
#define BMP390_OSR                0x1C       //setting p&t measurement
#define BMP390_ODR                0x1D       //config output data rates
#define BMP390_CMD                0x7E

#define BMP390_NVM_PAR_P11            0x45
#define BMP390_NVM_PAR_P10            0x44
#define BMP390_NVM_PAR_P9_H           0x43
#define BMP390_NVM_PAR_P9_L           0x42
#define BMP390_NVM_PAR_P8             0x41
#define BMP390_NVM_PAR_P7             0x40
#define BMP390_NVM_PAR_P6_H           0x3F
#define BMP390_NVM_PAR_P6_L           0x3E
#define BMP390_NVM_PAR_P5_H           0x3D
#define BMP390_NVM_PAR_P5_L           0x3C
#define BMP390_NVM_PAR_P4             0x3B
#define BMP390_NVM_PAR_P3             0x3A
#define BMP390_NVM_PAR_P2_H           0x39
#define BMP390_NVM_PAR_P2_L           0x38
#define BMP390_NVM_PAR_P1_H           0x37
#define BMP390_NVM_PAR_P1_L           0x36

#define BMP390_NVM_PAR_T3             0x35
#define BMP390_NVM_PAR_T2_H           0x34
#define BMP390_NVM_PAR_T2_L           0x33
#define BMP390_NVM_PAR_T1_H           0x32
#define BMP390_NVM_PAR_T1_L           0x31

#define Total_Number_32 4294967296.0
#define Total_Number_30 1073741824.0
#define Total_Number_29 536870912.0
#define Total_Number_24 16777216.0
#define Total_Number_20 1048576.0
#define Total_Number_16 65536.0
#define Total_Number_15 32768.0
#define Total_Number_14 16384.0
#define Total_Number_12 4096.0
#define Total_Number_8 256.0
#define Total_Number_6 64.0
#define Total_Number_5 32.0
#define Total_Number_1 2.0
 
#define Total_Number_Neg_8 0.00390625
#define Total_Number_Neg_3 0.125

#ifndef BMP390_PRT
#define BMP390_PRT(...)   \
do {\
    printf("[BMP390_PRT][%s][%5d] ", __FUNCTION__, __LINE__);\
    printf(__VA_ARGS__);\
} while(0)
#endif

void bmp390_i2c_init(void);
void bmp390_init(void);
void Parameter_Reading(int *Pressure_Para, int *Temperature_Para);
void bmp390_run_once(double *Correcting_Temp, double *Correcting_Press);

#endif
