#include "application.h"
#include <stm32f2xx_rcc.h>
#include <stm32f2xx_gpio.h>
#include <stm32f2xx_tim.h>
#include <misc.h>


SYSTEM_MODE(MANUAL);


extern "C" void TIM4_IRQHandler()
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update))
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
        GPIO_ToggleBits(GPIOB, GPIO_Pin_7);
    }
}
// Board LED is on Port PA13 = D7
// Board LED is on Port PB7 = D0

void EnableTimerInterrupt()
{
    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = TIM4_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
    nvicStructure.NVIC_IRQChannelSubPriority = 0;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicStructure);
}

void InitializeTimer()
{
  // Ref: https://visualgdb.com/tutorials/arm/stm32/timers/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  // TIM4 initialization overflow every 500ms
  // TIM4 by default has clock of 84MHz
  // Here, we must set value of prescaler and period,
  // so update event is 0.5Hz or 500ms
  // Update Event (Hz) = timer_clock / ((TIM_Prescaler + 1) *
  // (TIM_Period + 1))
  // Update Event (Hz) = 84MHz / ((4199 + 1) * (9999 + 1)) = 2 Hz

  TIM_TimeBaseInitTypeDef timerInitStructure;
  timerInitStructure.TIM_Prescaler = 4199; // 1 count = 1 ms
  timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  timerInitStructure.TIM_Period = 500;
  timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  timerInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM4, &timerInitStructure);
  TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM4, ENABLE);
}

void InitializeLED()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; // Enable pin 7
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; // in Output mode
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // @ 50MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // Push-pull
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // No pull-up resistor
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void setup() {
  InitializeLED();
  InitializeTimer();
  EnableTimerInterrupt();
}

void loop() {
  if (Particle.connected() == false) {
    Particle.connect();
  }
  // Particle.process();
  // GPIO_ToggleBits(GPIOB, GPIO_Pin_7);
  delay(1000);
}
