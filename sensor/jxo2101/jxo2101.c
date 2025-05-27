#include "gd32f10x.h"
#include "systick.h"

#include "common.h"
#include "jxo2101.h"

#if 0
uint8_t rxbuffer[7];
uint8_t rx_size = 7;
uint16_t rxcount = 0;
__IO bool jxo2101_flag = FALSE;

void jxo2101_uart_init(void)
{
  gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);   // PC10 TX
  gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);  // PC11 RX

  usart_deinit(UART3);    // UART3

  usart_baudrate_set(UART3, 9600);  // 设置波特率9600
  usart_word_length_set(UART3, USART_WL_8BIT);  // 8位数据
  usart_stop_bit_set(UART3, USART_STB_1BIT);    // 1位停止位
  usart_parity_config(UART3, USART_PM_NONE);    // 无奇偶校验
  usart_hardware_flow_rts_config(UART3, USART_RTS_DISABLE);  // 禁用硬件流控制
  usart_hardware_flow_cts_config(UART3, USART_CTS_DISABLE);  // 禁用硬件流控制
  usart_receive_config(UART3, USART_RECEIVE_ENABLE);  // 使能接收
  usart_transmit_config(UART3, USART_TRANSMIT_ENABLE);  // 使能发送

  usart_enable(UART3);    // 使能UART3
	
	// 开启 UART3 中断
	nvic_irq_enable(UART3_IRQn, 2, 0);  // 设置优先级
  usart_interrupt_enable(UART3, USART_INT_RBNE);
}

float parse_o2_data(uint8_t *data)
{
  int o2_raw = 0;
  float o2_value = 0;
	
  o2_raw = (data[3] << 8) | data[4]; // 高8位和低8位拼接成O2浓度
  o2_value = o2_raw * 0.1;
  
  return o2_value;
}

void jxo2101_read_o2_value(void)
{
  uint8_t command[] = {0x01, 0x03, 0x00, 0x02, 0x00, 0x01, 0x25, 0xCA};

  uart_send_data(UART3, command, sizeof(command));
  jxo2101_flag = TRUE;
}

void jxo2101_run_once(void)
{
  if (!jxo2101_flag) {
    jxo2101_read_o2_value();
  }
}

void UART3_IRQHandler(void)
{
  if (jxo2101_flag) {
    if(RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE)){
      /* receive data */
      rxbuffer[rxcount++] = usart_data_receive(UART3);
      if (rxcount == rx_size) {
				float o2_value = parse_o2_data(rxbuffer);
				JXO2101_PRT("Oxygen Concentration: %.1f\r\n", o2_value);
        jxo2101_flag = FALSE;
        rxcount = 0;
      }
    }
  }
}
#endif

void jxo2101_uart_init(void)
{
  gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);   // PC10 TX
  gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);  // PC11 RX

  usart_deinit(UART3);    // UART3

  usart_baudrate_set(UART3, 9600);  // 设置波特率9600
  usart_word_length_set(UART3, USART_WL_8BIT);  // 8位数据
  usart_stop_bit_set(UART3, USART_STB_1BIT);    // 1位停止位
  usart_parity_config(UART3, USART_PM_NONE);    // 无奇偶校验
  usart_hardware_flow_rts_config(UART3, USART_RTS_DISABLE);  // 禁用硬件流控制
  usart_hardware_flow_cts_config(UART3, USART_CTS_DISABLE);  // 禁用硬件流控制
  usart_receive_config(UART3, USART_RECEIVE_ENABLE);  // 使能接收
  usart_transmit_config(UART3, USART_TRANSMIT_ENABLE);  // 使能发送

  usart_enable(UART3);    // 使能UART3
}

float parse_o2_data(uint8_t *data)
{
  int o2_raw = 0;
  float o2_value = 0;
	
  o2_raw = (data[3] << 8) | data[4]; // 高8位和低8位拼接成O2浓度
  o2_value = o2_raw * 0.1;
  
  return o2_value;
}

float jxo2101_read_o2_value(void)
{
  uint8_t command[] = {0x01, 0x03, 0x00, 0x02, 0x00, 0x01, 0x25, 0xCA};
  uint8_t buffer[7];
  float o2_value;

  uart_send_data(UART3, command, sizeof(command));

  uart_receive_data(UART3, buffer, sizeof(buffer));

  o2_value = parse_o2_data(buffer);
  return o2_value;
}

void jxo2101_run_once(float *o2_value)
{
  *o2_value = jxo2101_read_o2_value();
  JXO2101_PRT("Oxygen Concentration: %.1f%%\r\n", *o2_value);
}
