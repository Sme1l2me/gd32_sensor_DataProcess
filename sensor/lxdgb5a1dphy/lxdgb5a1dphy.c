#include <stdio.h>
#include <math.h>
#include "gd32f10x.h"
#include "systick.h"

#include "common.h"
#include "lxdgb5a1dphy.h"

#define VREF     5

void lxdgb5a1dphy_adc_init(void)
{
  gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_1);  // PA1 作为 ADC 输入

  adc_deinit(ADC0);
  adc_mode_config(ADC_MODE_FREE);
  adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
  adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1);
  adc_regular_channel_config(ADC0, 0, LXDGB5A1DPHY_ADC_CHANNEL, ADC_SAMPLETIME_55POINT5);
  
  adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
  adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
  
  adc_enable(ADC0);
  delay_1ms(1);
  adc_calibration_enable(ADC0);
}

uint16_t lxdgb5a1dphy_adc_read(void)
{
  adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
  while (!adc_flag_get(ADC0, ADC_FLAG_EOC));
  return adc_regular_data_read(ADC0);
}

float adc_to_lux(uint16_t adc_value)
{
  float v_adc = (adc_value * VREF) / 4095;  // ADC 转换为电压
  float lux = (v_adc / 0.5) * 1000;  // 计算照度（Lux）
  return lux;
}

void lxdgb5a1dphy_run_once(float *lux)
{
  uint16_t lxdgb5a1dphy_adc_value;

  lxdgb5a1dphy_adc_value = lxdgb5a1dphy_adc_read();

  *lux = adc_to_lux(lxdgb5a1dphy_adc_value);
  LXDGB5A1DPHY_PRT("Lux:%.2f lx\r\n", *lux);
}
