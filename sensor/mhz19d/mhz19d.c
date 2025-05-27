#include "gd32f10x.h"
#include "systick.h"

#include "common.h"
#include "mhz19d.h"

#if 0
uint8_t mhz19d_rxbuffer[9];
uint8_t mhz19d_rx_size = 9;
uint16_t mhz19d_rxcount = 0; 
__IO bool mhz19d_flag = FALSE;

void mhz19d_uart_init(void)
{
  gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);   // PA9 TX
  gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);  // PA10 RX

  usart_deinit(USART0);    // 复位USART0

  usart_baudrate_set(USART0, 9600);
  usart_word_length_set(USART0, USART_WL_8BIT);  // 8位数据
  usart_stop_bit_set(USART0, USART_STB_1BIT);    // 1位停止位
  usart_parity_config(USART0, USART_PM_NONE);    // 无奇偶校验
  usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);  // 禁用硬件流控制
  usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);  // 禁用硬件流控制
  usart_receive_config(USART0, USART_RECEIVE_ENABLE);  // 使能接收
  usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);  // 使能发送

  usart_enable(USART0);    // 使能USART0

  // 开启 USART0 中断
	nvic_irq_enable(USART0_IRQn, 2, 0);  // 设置优先级
  usart_interrupt_enable(USART0, USART_INT_RBNE);
}

uint8_t calculate_checksum(uint8_t *data) {
	uint8_t sum = 0;
	for (int i = 1; i < 8; i++) { // D1 ~ D7
		sum += data[i];
	}
	return (uint8_t)(~sum + 1); // 取反加 1
}

int parse_co2_data(uint8_t* data) {
	// 校验头部是否正确
	if (data[0] != 0xFF || data[1] != 0x86) {
		MHZ19D_PRT("Error: Invalid packet header!\r\n");
		return -1;
	}

	// 计算校验值
	uint8_t checksum = calculate_checksum(data);
	if (checksum != data[8]) {
		MHZ19D_PRT("Error: Checksum mismatch! Expected: 0x%02X, Received: 0x%02X\r\n", checksum, data[8]);
		return -1;
	}
	
  int co2_ppm = (data[2] << 8) | data[3]; // 高8位和低8位拼接成CO2浓度
  return co2_ppm;
}

void mhz19d_set_range(void)
{
  uint8_t command[] = {0xFF, 0x01, 0x99, 0x00, 0x00, 0x00, 0x13, 0x88, 0xCB};

  uart_send_data(MHZ19D_UART, command, sizeof(command));
}

void mhz19d_read_co2_value(void)
{
  uint8_t command[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};

  uart_send_data(MHZ19D_UART, command, sizeof(command));
	for(int i = 0; i < sizeof(command); i++){
    printf("send data: %02x\r\n", command[i]);
  }
}

void mhz19d_run_once(void)
{
  if(!mhz19d_flag) {
    mhz19d_read_co2_value();
    mhz19d_flag = TRUE;
  }
}

void USART0_IRQHandler(void)
{
  if (mhz19d_flag) {
    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)) {
      /* receive data */
      mhz19d_rxbuffer[mhz19d_rxcount++] = usart_data_receive(USART0);
      if (mhz19d_rxcount >= mhz19d_rx_size) {
        mhz19d_flag = FALSE;
        mhz19d_rxcount = 0;
        int co2_ppm = parse_co2_data(mhz19d_rxbuffer);
        MHZ19D_PRT("CO2 Concentration: %d ppm\r\n", co2_ppm);
      }
    }
  }
}
#endif

void mhz19d_uart_init(void)
{
  gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);   // PA9 TX
  gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);  // PA10 RX

  usart_deinit(USART0);    // 复位USART0

  usart_baudrate_set(USART0, 9600);
  usart_word_length_set(USART0, USART_WL_8BIT);  // 8位数据
  usart_stop_bit_set(USART0, USART_STB_1BIT);    // 1位停止位
  usart_parity_config(USART0, USART_PM_NONE);    // 无奇偶校验
  usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);  // 禁用硬件流控制
  usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);  // 禁用硬件流控制
  usart_receive_config(USART0, USART_RECEIVE_ENABLE);  // 使能接收
  usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);  // 使能发送

  usart_enable(USART0);    // 使能USART0
}

void mhz19d_hd_disable(void)
{
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
	
	gpio_bit_set(GPIOA, GPIO_PIN_8);
}

uint8_t calculate_checksum(uint8_t *data) {
	uint8_t sum = 0;
	for (int i = 1; i < 8; i++) { // D1 ~ D7
		sum += data[i];
	}
	return (uint8_t)(~sum + 1); // 取反加 1
}

int parse_co2_data(uint8_t* data) {
	// 校验头部是否正确
	if (data[0] != 0xFF || data[1] != 0x86) {
		MHZ19D_PRT("Error: Invalid packet header!\r\n");
		return -1;
	}

	// 计算校验值
	uint8_t checksum = calculate_checksum(data);
	if (checksum != data[8]) {
		MHZ19D_PRT("Error: Checksum mismatch! Expected: 0x%02X, Received: 0x%02X\r\n", checksum, data[8]);
		return -1;
	}
	
  int co2_ppm = (data[2] << 8) | data[3]; // 高8位和低8位拼接成CO2浓度
  return co2_ppm;
}

void mhz19d_set_range(void)
{
  uint8_t command[] = {0xFF, 0x01, 0x99, 0x00, 0x00, 0x00, 0x13, 0x88, 0xCB};
	
  uart_send_data(MHZ19D_UART, command, sizeof(command));
}

int mhz19d_read_co2_value(void)
{
  uint8_t command[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  uint8_t rx_buffer[9];
  int co2_ppm;
	
  uart_send_data(MHZ19D_UART, command, sizeof(command));
	
  uart_receive_data(MHZ19D_UART, rx_buffer, sizeof(rx_buffer));

  co2_ppm = parse_co2_data(rx_buffer);
  return co2_ppm;
}

void mhz19d_run_once(int *co2_ppm)
{
  *co2_ppm = mhz19d_read_co2_value();
  MHZ19D_PRT("CO2 Concentration: %d ppm\r\n", *co2_ppm);
}
