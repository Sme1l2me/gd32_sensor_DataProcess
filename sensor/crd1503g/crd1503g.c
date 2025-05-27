#include <stdio.h>
#include <math.h>
#include "gd32f10x.h"
#include "systick.h"

#include "common.h"
#include "crd1503g.h"

volatile uint32_t pulse_count = 0;      // 计数脉冲
volatile uint32_t pulse_value = 0;      // 每5秒的脉冲数
volatile uint8_t pulse_ready_flag = 0;  // 标记是否有新脉冲数据

void crd1503g_timer2_init(void)
{
  timer_deinit(TIMER2);

  timer_parameter_struct timer_initpara;
  timer_initpara.prescaler = 10000 - 1;
  timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
  timer_initpara.counterdirection = TIMER_COUNTER_UP;
  timer_initpara.period = 50000 - 1;
  timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
  timer_initpara.repetitioncounter = 0;
  timer_init(TIMER2, &timer_initpara);

  timer_interrupt_flag_clear(TIMER2, TIMER_INT_UP);
  /* 使能 TIMER2 中断 */
  timer_interrupt_enable(TIMER2, TIMER_INT_UP);
  nvic_irq_enable(TIMER2_IRQn, 2, 0);

  /* 启动 TIMER2 */
  timer_enable(TIMER2);
}

void crd1503g_exti_init(void)
{
  gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_12);

  gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_12);

  exti_init(EXTI_12, EXTI_INTERRUPT, EXTI_TRIG_RISING);

  exti_interrupt_flag_clear(EXTI_12);

  nvic_irq_enable(EXTI10_15_IRQn, 2, 0);
}

void EXTI10_15_IRQHandler(void)
{
  if (exti_interrupt_flag_get(EXTI_12) != RESET) {
    pulse_count++;  // 计数脉冲
    exti_interrupt_flag_clear(EXTI_12);  // 清除中断标志
  }
}

void TIMER2_IRQHandler(void)
{
  if (timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP) != RESET)
  {
    pulse_value = pulse_count;
    pulse_ready_flag = 1;
    /* 清除计数，准备统计下一个周期 */
    pulse_count = 0;

    /* 清除 TIMER2 中断标志 */
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
  }
}
