#include "application.h"
#include <stm32f2xx_rcc.h>
#include <stm32f2xx_gpio.h>
#include <stm32f2xx_tim.h>
#include <misc.h>

#define SYSCORECLOCK	60000000UL   // photon - 60Mhz timer clock
const uint16_t SIT_PRESCALERu = (uint16_t)(SYSCORECLOCK / 1000000UL) - 1;	//To get TIM counter clock = 1MHz
const uint16_t SIT_PRESCALERm = (uint16_t)(SYSCORECLOCK / 2000UL) - 1;	//To get TIM counter clock = 2KHz

SYSTEM_MODE(MANUAL);

void Wiring_TIM4_Interrupt_Handler_override()
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    GPIO_ToggleBits(GPIOB, GPIO_Pin_7);
	}
}


void EnableTimerInterrupt()
{
    if (!attachSystemInterrupt(SysInterrupt_TIM4_Update, Wiring_TIM4_Interrupt_Handler_override)) ;	//error
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


  TIM_TimeBaseInitTypeDef timerInitStructure;
  timerInitStructure.TIM_Prescaler = SIT_PRESCALERm;
  timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  timerInitStructure.TIM_Period = 2000;  // 2000 * 0.5 ms = 1 sec
  timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV4;
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
  EnableTimerInterrupt();
  InitializeTimer();
}

void loop() {
  if (Particle.connected() == false) {
    Particle.connect();
  }
  Particle.process();
  delay(1000);
}
