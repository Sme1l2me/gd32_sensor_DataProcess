#include "gd32f10x.h"
#include "systick.h"

#include "common.h"
#include "msm261s4030h0r.h"

#define BUFFER_SIZE 4
uint32_t i2s_rx_buffer[BUFFER_SIZE];

void msm261s4030h0r_dma_init(void)
{
	/* DMA 配置 */
	dma_parameter_struct dma_init_struct;
	dma_struct_para_init(&dma_init_struct);

	dma_deinit(DMA0, DMA_CH4);

	dma_init_struct.periph_addr = (uint32_t)&SPI_DATA(SPI1);
	dma_init_struct.memory_addr = (uint32_t)i2s_rx_buffer;
	dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_32BIT;
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_32BIT;
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_init_struct.number = BUFFER_SIZE;
	dma_init_struct.priority = DMA_PRIORITY_LOW;
	dma_init(DMA0, DMA_CH4, &dma_init_struct);

	/* 使能 DMA 循环模式 */
	dma_circulation_enable(DMA0, DMA_CH4);
	dma_memory_to_memory_disable(DMA0, DMA_CH4);
}

void msm261s4030h0r_spi_init(void)
{
	/* 复位 SPI1 */
  spi_i2s_deinit(SPI1);

  /* 配置 I²S 只读模式 */
  i2s_init(SPI1, I2S_MODE_MASTERRX, I2S_STD_PHILIPS, I2S_CKPL_LOW);
  i2s_psc_config(SPI1, I2S_AUDIOSAMPLE_48K, I2S_FRAMEFORMAT_DT24B_CH32B, I2S_MCKOUT_DISABLE);
}

void msm261s4030h0r_init(void)
{
	/* 配置 I²S 引脚: PB12 (WS), PB13 (SCK), PB15 (SD) */
  gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15);
	
//	msm261s4030h0r_dma_init();
	
	msm261s4030h0r_spi_init();
	
//	spi_dma_enable(SPI1, SPI_DMA_RECEIVE);

//  dma_channel_enable(DMA0, DMA_CH4);

  /* 使能 I²S */
  i2s_enable(SPI1);
}

int	msm261s4030h0r_get_right_channal_data(void)
{
  uint32_t right_raw_data;
  int audio_value;

  while(!spi_i2s_flag_get(SPI1, SPI_FLAG_RBNE));
  for (int i = 0; i < BUFFER_SIZE; i++) {
    i2s_rx_buffer[i] = spi_i2s_data_receive(SPI1);
  }
  right_raw_data = (i2s_rx_buffer[2] << 8) + (i2s_rx_buffer[3] >> 8); // 获取右声道 24-bit 数据
  if (right_raw_data & 0x800000) {
    audio_value = right_raw_data | 0xFF000000;  // 符号扩展
  } else {
    audio_value = right_raw_data;
  }
	return audio_value;
}

void msm261s4030h0r_run_once(int *audio_value)
{
  *audio_value = msm261s4030h0r_get_right_channal_data();
	MSM261S4030H0R_PRT("audio value : %d\r\n", *audio_value);
}
