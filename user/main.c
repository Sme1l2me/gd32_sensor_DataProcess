#include "gd32f10x.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "main.h"

#include "common.h"

Sensor_D_T sensor_data;

void uart_config(void);

uint16_t crc16_modbus(const uint8_t *data, uint16_t length)
{
  uint16_t crc = 0xFFFF;

  for (uint16_t i = 0; i < length; i++) {
      crc ^= data[i];

      for (uint8_t j = 0; j < 8; j++) {
          if (crc & 0x0001)
              crc = (crc >> 1) ^ 0xA001;
          else
              crc >>= 1;
      }
  }
  return crc;
}

void sensor_data_pack(Sensor_D_T *sensor_data)
{
  Sensor_Frame_T frame;

  memset(&frame, 0, sizeof(Sensor_Frame_T));

  frame.head[0] = FRAME_HEADER1;
  frame.head[1] = FRAME_HEADER2;
  frame.tail[0] = FRAME_TAIL1;
  frame.tail[1] = FRAME_TAIL2;

  memcpy(&frame.sensor_data, sensor_data, sizeof(Sensor_D_T));
  frame.crc = crc16_modbus((uint8_t *)&frame.sensor_data, offsetof(Sensor_Frame_T, crc));
}

int main(void)
{
	/* configure systick */
	systick_config();
	
	/* enable sensor periph clk*/
	sensor_periph_clk_enable();
	
	sensor_power_enable();
	
	uart_config();
	
	sensor_init();

  memset(&sensor_data, 0, sizeof(Sensor_D_T));
	
	//warm up 60s
//	gm702b_warm_up();
//	
//	mhz19d_set_range();

  while(1) {
//    printf("test\r\n");
//    delay_1ms(100);
		/* i2c sensor */
//    aht20_run_once(&sensor_data.aht20_h_t_data.humidity, &sensor_data.aht20_h_t_data.temperature);

//    qmi8658a_run_once(&sensor_data.qmi8658a_a_g_data.ax, 
//                      &sensor_data.qmi8658a_a_g_data.ay,
//                      &sensor_data.qmi8658a_a_g_data.az,
//                      &sensor_data.qmi8658a_a_g_data.gx,
//                      &sensor_data.qmi8658a_a_g_data.gy,
//                      &sensor_data.qmi8658a_a_g_data.gz);
//		
//		bmp390_run_once(&sensor_data.bmp390_t_p_data.temperature,
//                    &sensor_data.bmp390_t_p_data.pressure);
//                    
//    /* uart sensor */	
//		pms9103s_run_once(&sensor_data.pms9103s_pm_data.pm1_0,
//											&sensor_data.pms9103s_pm_data.pm2_5,
//											&sensor_data.pms9103s_pm_data.pm10);

//    mhz19d_run_once(&sensor_data.mhz19d_co2_ppm);

//    jxo2101_run_once(&sensor_data.jxo2101_o2_ppm);

//		/* i2s sensor */
//    msm261s4030h0r_run_once(&sensor_data.msm261s4030h0r_audio_value);

//		/* adc sensor */
//    gm702b_run_once(&sensor_data.gm702b_ppm);

    lxdgb5a1dphy_run_once(&sensor_data.lxdgb5a1dphy_lux);

//    cx1088_run_once(&sensor_data.cx1088_gas);

//		ersensor_run_once(&sensor_data.ersensor_value);
//    
//    /* it sensor */
//    if(pulse_ready_flag) {
//      __disable_irq();
//      sensor_data.crd1503g_pulse = pulse_value;
//      CRD1503G_PRT("pulse value:%d \r\n", sensor_data.crd1503g_pulse);
//      pulse_ready_flag = 0;
//      __enable_irq();
//    }

//    sensor_data_pack(&sensor_data);
//		m2313_run_once();			//****i2c addr bug
    delay_1ms(5000);
  }
}

void uart_config(void)
{
	gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
	gpio_init(GPIOD, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

	/* UART configure */
	usart_deinit(UART4);
	usart_baudrate_set(UART4, 115200U);
	usart_word_length_set(UART4, USART_WL_8BIT);
	usart_stop_bit_set(UART4, USART_STB_1BIT);
	usart_parity_config(UART4, USART_PM_NONE);
	usart_receive_config(UART4, USART_RECEIVE_ENABLE);
	usart_transmit_config(UART4, USART_TRANSMIT_ENABLE);
	usart_enable(UART4);
}

int fputc(int ch, FILE *f)
{
	usart_data_transmit(UART4, (uint8_t)ch);
	while(RESET == usart_flag_get(UART4, USART_FLAG_TBE));
	return ch;
}


