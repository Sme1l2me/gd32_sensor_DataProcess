#include <stdio.h>
#include <math.h>
#include "gd32f10x.h"
#include "systick.h"

#include "common.h"
#include "e_r_sensor.h"

void ersensor_adc_init(void)
{
  gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0);  // PC0 作为 ADC 输入

  adc_deinit(ADC0);
  adc_mode_config(ADC_MODE_FREE);
  adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
  adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1);
  adc_regular_channel_config(ADC0, 0, ERSENSOR_ADC_CHANNEL, ADC_SAMPLETIME_55POINT5);

  adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
  adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

  adc_enable(ADC0);
  delay_1ms(1);
  adc_calibration_enable(ADC0);
}

uint16_t ersensor_adc_read(void)
{
  adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
  while (!adc_flag_get(ADC0, ADC_FLAG_EOC));
  return adc_regular_data_read(ADC0);
}

float ersensor_read_voltage(uint16_t adc_value) {
  float voltage = (5.0 * adc_value) / 4095.0;  // 计算 VRL 电压
  return voltage;
}

void ersensor_run_once(float *radiation_voltage_value)
{
  uint16_t adc_value;

  adc_value = ersensor_adc_read();
  *radiation_voltage_value = ersensor_read_voltage(adc_value);

  ERSENSOR_PRT("Electromagnetic radiation voltage value: %.1f\r\n", *radiation_voltage_value);
}
