#include <stdio.h>
#include "systick.h"
#include "gd32f10x.h"
#include "common.h"

ErrStatus hxtal_stabilization;

void sensor_periph_clk_enable(void)
{
  rcu_periph_clock_enable(RCU_AF);
  
  rcu_periph_clock_enable(RCU_I2C0);
  rcu_periph_clock_enable(RCU_I2C1);
	
	rcu_periph_clock_enable(RCU_TIMER2);

  rcu_periph_clock_enable(RCU_ADC0);

  rcu_periph_clock_enable(RCU_USART0);
  rcu_periph_clock_enable(RCU_USART1);
  rcu_periph_clock_enable(RCU_UART3);
  rcu_periph_clock_enable(RCU_UART4);

  rcu_periph_clock_enable(RCU_SPI1);
	
	rcu_periph_clock_enable(RCU_GPIOA);
  rcu_periph_clock_enable(RCU_GPIOB);
  rcu_periph_clock_enable(RCU_GPIOC);
  rcu_periph_clock_enable(RCU_GPIOD);
}

void sensor_power_enable(void)
{
  gpio_pin_remap_config(GPIO_SWJ_NONJTRST_REMAP, ENABLE); 
	
  gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);	//PB4 5V
  gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);	//PB5 3.3V
	
  gpio_bit_set(GPIOB, GPIO_PIN_4);
  gpio_bit_set(GPIOB, GPIO_PIN_5);
}

void sensor_init(void)
{
//	aht20_i2c_init();
//	delay_1ms(5);

//  qmi8658a_i2c_init();
//  qmi8658a_init();
//	delay_1ms(5);

//	bmp390_i2c_init();
//	delay_1ms(5);

//	pms9103s_uart_init();
//	pms9103s_start();
//	delay_1ms(5);

//	jxo2101_uart_init();
//	delay_1ms(5);
//	
//	mhz19d_uart_init();
//	mhz19d_hd_disable();
//  mhz19d_set_range();
//	delay_1ms(5);

//	gm702b_adc_init();
//	delay_1ms(5);

	lxdgb5a1dphy_adc_init();
	delay_1ms(5);

//	cx1088_adc_init();
//	cx1088_enable();
//	cx1088_disable();
//	delay_1ms(5);

//	ersensor_adc_init();
//	delay_1ms(5);

//  msm261s4030h0r_init();
//	delay_1ms(5);

//  crd1503g_timer2_init();
//  crd1503g_exti_init();

//	m2313_i2c_init();//bug
}

void uart_send_data(uint32_t uartx, uint8_t *data, uint8_t length)
{
  while (usart_flag_get(uartx, USART_FLAG_TBE) == RESET);

  for (uint8_t i = 0; i < length; i++) {
    usart_data_transmit(uartx, data[i]);    // 发送数据
    while (usart_flag_get(uartx, USART_FLAG_TBE) == RESET);  // 等待发送完成
  }

  while (usart_flag_get(uartx, USART_FLAG_TC) == RESET);
}

void uart_receive_data(uint32_t uartx, uint8_t *data, uint8_t length)
{
	uint8_t i;
	
	 /* clear the data register */
  usart_data_receive(uartx);
	
	for (i = 0; i < length; i++) {
		while (usart_flag_get(uartx, USART_FLAG_RBNE) == RESET);
		data[i] = usart_data_receive(uartx);
	}
}

void i2c_master_transmitter(uint32_t i2c_periph, uint32_t i2c_addr, uint8_t *data, uint8_t length)
{
  /* Wait until I2C bus is idle */
  while(i2c_flag_get(i2c_periph, I2C_FLAG_I2CBSY));

  /* Send START condition */
  i2c_start_on_bus(i2c_periph);
  while(!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND));

  /* Send slave address with write operation */
  i2c_master_addressing(i2c_periph, i2c_addr, I2C_TRANSMITTER);

  /* Wait until address is sent */
  while(!i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND));

  /* Clear ADDSEND flag */
  i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);

  /* Send command array */
  for (uint8_t i = 0; i < length; i++) {
      while(!i2c_flag_get(i2c_periph, I2C_FLAG_TBE));
      i2c_data_transmit(i2c_periph, data[i]);
  }
  while (!i2c_flag_get(i2c_periph, I2C_FLAG_TBE));

  /* Send STOP condition */
  i2c_stop_on_bus(i2c_periph);
  while(I2C_CTL0(i2c_periph) & 0x0200);
}

void i2c_master_receiver(uint32_t i2c_periph, uint32_t i2c_addr, uint8_t *data, uint8_t length)
{
  if (length == 2) {
    while(i2c_flag_get(i2c_periph, I2C_FLAG_I2CBSY));

    I2C_CTL0(i2c_periph) |= I2C_CTL0_POAP;
    /* 发送起始信号 */
    i2c_start_on_bus(i2c_periph);
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND));
    /* 发送从机地址 */
    i2c_master_addressing(i2c_periph, i2c_addr, I2C_RECEIVER);
    /* 等待地址发送完成 */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND));
    /* send a NACK for the last data byte */
    i2c_ack_config(i2c_periph, I2C_ACK_DISABLE);
    /* clear ADDSEND bit */
    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);

    /* 等待 BTC 置 1 */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_BTC));
		/* STOP位 置 1 */
		i2c_stop_on_bus(i2c_periph);
    /* 读取两个字节 */
		while(!i2c_flag_get(i2c_periph, I2C_FLAG_RBNE));
    data[0] = i2c_data_receive(i2c_periph);
    data[1] = i2c_data_receive(i2c_periph);
  } 
  else if (length == 1) {
    while(i2c_flag_get(i2c_periph, I2C_FLAG_I2CBSY));

    I2C_CTL0(i2c_periph) |= I2C_CTL0_POAP;
    /* 发送起始信号 */
    i2c_start_on_bus(i2c_periph);
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND));
    /* 发送从机地址 */
    i2c_master_addressing(i2c_periph, i2c_addr, I2C_RECEIVER);
    /* 等待地址发送完成 */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND));
		/* send a NACK for the last data byte */
    i2c_ack_config(i2c_periph, I2C_ACK_DISABLE);
    /* clear ADDSEND bit */
    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);
		/* STOP位 置 1 */
		i2c_stop_on_bus(i2c_periph);
    /* 读取1个字节 */
		while(!i2c_flag_get(i2c_periph, I2C_FLAG_RBNE));
    data[0] = i2c_data_receive(i2c_periph);
  }
  else {
    while(i2c_flag_get(i2c_periph, I2C_FLAG_I2CBSY));
    i2c_start_on_bus(i2c_periph);
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_SBSEND));

    i2c_master_addressing(i2c_periph, i2c_addr, I2C_RECEIVER);
    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(i2c_periph, I2C_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c_flag_clear(i2c_periph, I2C_FLAG_ADDSEND);

    for (uint8_t i = 0; i < length; i++) {
      /* wait until the RBNE bit is set */
      if (i == length - 2) {
        /* wait until the second last data byte is received into the shift register */
        while(!i2c_flag_get(i2c_periph, I2C_FLAG_BTC));
        /* send a NACK for the last data byte */
        i2c_ack_config(i2c_periph, I2C_ACK_DISABLE);
      }
      while(!i2c_flag_get(i2c_periph, I2C_FLAG_RBNE));
      data[i] = i2c_data_receive(i2c_periph);
    }
		/* send a stop condition to I2C bus */
		i2c_stop_on_bus(i2c_periph);
		/* wait until stop condition generate */
		while(I2C_CTL0(i2c_periph) & 0x0200);
  }
}

void i2c_bus_reset(void) 
{
    // 先关闭 I2C 以释放 GPIO
    i2c_deinit(I2C1);
    
    // 设为普通 GPIO，手动拉高 SCL 释放挂起的设备
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10 | GPIO_PIN_11);
    
    for (int i = 0; i < 10; i++) {
        gpio_bit_write(GPIOB, GPIO_PIN_6, SET);  // 拉高 SCL
        for (volatile int j = 0; j < 1000; j++); // 短暂延迟
        gpio_bit_write(GPIOB, GPIO_PIN_6, RESET); // 拉低 SCL
        for (volatile int j = 0; j < 1000; j++);
    }

    // 重新启用 I2C 功能
    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_10 | GPIO_PIN_11);
    i2c_clock_config(I2C1, 100000, I2C_DTCY_2);
    i2c_enable(I2C1);
}

void i2c_scan_bus(bool scan_10bit) {
    printf("Scanning I2C bus (%s mode)...\r\n", scan_10bit ? "10-bit" : "7-bit");

    uint16_t start_addr = scan_10bit ? 0x000 : 0x08;
    uint16_t end_addr = scan_10bit ? 0x3FF : 0x78;

    for (uint16_t address = start_addr; address <= end_addr; address++) {
        int timeout = 10000;

        // 确保总线空闲
        if (i2c_flag_get(I2C1, I2C_FLAG_I2CBSY)) {
            printf("I2C Bus Busy! Resetting...\r\n");
            i2c_bus_reset();
            continue;
        }

        i2c_start_on_bus(I2C1);  // 发送起始信号
        while (!i2c_flag_get(I2C1, I2C_FLAG_SBSEND)) {
            if (--timeout == 0) {
                printf("I2C Start Timeout at addr 0x%X\r\n", address);
                i2c_stop_on_bus(I2C1);
                goto NEXT_ADDRESS;
            }
        }

        // 发送设备地址
        if (scan_10bit) {
            // 10-bit 地址模式：发送前 7 位 (0b11110XX) + 后 3 位
            uint8_t high_addr = 0xF0 | ((address >> 8) & 0x06);
            uint8_t low_addr = address & 0xFF;

            i2c_master_addressing(I2C1, high_addr, I2C_TRANSMITTER);
            while (!i2c_flag_get(I2C1, I2C_FLAG_ADDSEND)) {
                if (--timeout == 0) {
                    printf("I2C Address Timeout at 0x%X\r\n", address);
                    i2c_stop_on_bus(I2C1);
                    goto NEXT_ADDRESS;
                }
            }

            // 发送 10-bit 低 8 位地址
            i2c_data_transmit(I2C1, low_addr);
            while (!i2c_flag_get(I2C1, I2C_FLAG_TBE));  // 等待数据传输完成
        } else {
            // 7-bit 地址模式
            i2c_master_addressing(I2C1, (address << 1), I2C_TRANSMITTER);
        }

        timeout = 10000;
        while (!(i2c_flag_get(I2C1, I2C_FLAG_ADDSEND) || i2c_flag_get(I2C1, I2C_FLAG_AERR))) {
            if (--timeout == 0) {
                printf("I2C Address Timeout at 0x%X\r\n", address);
                i2c_stop_on_bus(I2C1);
                goto NEXT_ADDRESS;
            }
        }

        if (i2c_flag_get(I2C1, I2C_FLAG_ADDSEND)) {  // 设备存在
            printf("I2C device found at address: 0x%X\r\n", address);
            i2c_flag_clear(I2C1, I2C_FLAG_ADDSEND);
        } else {  // 设备未响应，清除错误标志
            i2c_flag_clear(I2C1, I2C_FLAG_AERR);
        }

NEXT_ADDRESS:
        // 发送 STOP 释放总线
        i2c_stop_on_bus(I2C1);
        while (i2c_flag_get(I2C1, I2C_FLAG_I2CBSY));  // 确保总线空闲
    }

    printf("I2C scan complete.\r\n");
}
