#include "gd32f10x.h"
#include "systick.h"

#include "common.h"
#include "pms9103s.h"

#define PMS9103S_DATA_LENGTH 32

void pms9103s_uart_init(void)
{
  gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);   // PA2 TX
  gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);  // PA3 RX

  usart_deinit(USART1);    // USART1

  usart_baudrate_set(USART1, 9600);  // 设置波特率9600
  usart_word_length_set(USART1, USART_WL_8BIT);  // 8位数据
  usart_stop_bit_set(USART1, USART_STB_1BIT);    // 1位停止位
  usart_parity_config(USART1, USART_PM_NONE);    // 无奇偶校验
  usart_hardware_flow_rts_config(USART1, USART_RTS_DISABLE);  // 禁用硬件流控制
  usart_hardware_flow_cts_config(USART1, USART_CTS_DISABLE);  // 禁用硬件流控制
  usart_receive_config(USART1, USART_RECEIVE_ENABLE);  // 使能接收
  usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);  // 使能发送

  usart_enable(USART1);    // 使能USART1
}

void pms9103s_start(void)
{
  gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);  // PC5 set
  gpio_bit_set(GPIOC, GPIO_PIN_5); 
}

void parse_sensor_data(uint8_t *data, uint16_t *pm1_0, uint16_t *pm2_5, uint16_t *pm10)
{
  uint16_t checksum = 0;
  for (int i = 0; i < PMS9103S_DATA_LENGTH - 2; i++) {
    checksum += data[i];
  }
	
  uint16_t received_checksum = (data[30] << 8) | data[31];

  if (checksum != received_checksum) {
    PMS9103S_PRT("Checksum error!\r\n");
    return;
  }
  
  if (data[0] == 0x42 && data[1] == 0x4d) {
    *pm1_0 = (data[4] << 8) | data[5];
    *pm2_5 = (data[6] << 8) | data[7];
    *pm10  = (data[8] << 8) | data[9];
  }
}

void pms9103s_run_once(uint16_t *pm1_0, uint16_t *pm2_5, uint16_t *pm10)
{
  uint8_t rx_buffer[PMS9103S_DATA_LENGTH];
	uint8_t command[] = {0x42, 0x4d, 0xe2, 0x00, 0x00, 0x01, 0x71};

  uart_send_data(PMS9103S_UART, command, sizeof(command));

  uart_receive_data(PMS9103S_UART, rx_buffer, PMS9103S_DATA_LENGTH);
  parse_sensor_data(rx_buffer, pm1_0, pm2_5, pm10);  // 解析数据
  
  PMS9103S_PRT("PM1.0: %d ug/m^3, PM2.5: %d ug/m^3, PM10: %d ug/m^3\r\n", *pm1_0, *pm2_5, *pm10);
}
