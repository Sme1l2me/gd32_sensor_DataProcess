#include <stdio.h>
#include <math.h>
#include "gd32f10x.h"
#include "systick.h"

#include "common.h"
#include "gm702b.h"

void gm702b_warm_up(void)
{
	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);	//PC9 2.5V
	gpio_bit_set(GPIOC, GPIO_PIN_9);
	delay_1ms(60000);
	gpio_bit_reset(GPIOC, GPIO_PIN_9);
}

void gm702b_adc_init(void)
{
  gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0);  // PA0 作为 ADC 输入

  adc_deinit(ADC0);
  adc_mode_config(ADC_MODE_FREE);
  adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
  adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1);
  adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_SENSOR, ADC_SAMPLETIME_55POINT5);
  
  adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
  adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
  
  adc_enable(ADC0);
  delay_1ms(1);
  adc_calibration_enable(ADC0);
}

uint16_t gm702b_adc_read(void)
{
  adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
  while (!adc_flag_get(ADC0, ADC_FLAG_EOC));
  return adc_regular_data_read(ADC0);
}

float gm702b_calculate_ppm(uint16_t adc_value)
{
  float voltage = (5.0 * adc_value) / 4095.0;  // 计算 ADC 电压
	float ppm = (150 * voltage) / (5 - voltage);
  return ppm;
}

void gm702b_run_once(float *COppm)
{
  uint16_t adc_value;

  adc_value = gm702b_adc_read();

  *COppm = gm702b_calculate_ppm(adc_value);
  GM702B_PRT("CO Concentration: %.2f ppm\r\n", *COppm);
}
