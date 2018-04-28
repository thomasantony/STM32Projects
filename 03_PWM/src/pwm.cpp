#include "application.h"
#include <stm32f2xx_rcc.h>
#include <stm32f2xx_gpio.h>
#include <stm32f2xx_tim.h>
#include <misc.h>
#include <math.h>

#define SYSCORECLOCK	60000000UL   // photon - 60Mhz timer clock
const uint16_t SIT_PRESCALERu = (uint16_t)(SYSCORECLOCK / 1000000UL) - 1;	//To get TIM counter clock = 1MHz
const uint16_t SIT_PRESCALERm = (uint16_t)(SYSCORECLOCK / 2000UL) - 1;	//To get TIM counter clock = 2KHz

SYSTEM_MODE(MANUAL);

void InitializePWM()
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  TIM_TimeBaseInitTypeDef timerInitStructure;
  timerInitStructure.TIM_Prescaler = SIT_PRESCALERu;
  timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  timerInitStructure.TIM_Period = 500;
  timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  timerInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM4, &timerInitStructure);
  TIM_Cmd(TIM4, ENABLE);

  TIM_OCInitTypeDef outputChannelInit = {0,};
  outputChannelInit.TIM_OCMode = TIM_OCMode_PWM1;
  outputChannelInit.TIM_Pulse = 250;
  outputChannelInit.TIM_OutputState = TIM_OutputState_Enable;
  outputChannelInit.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM4, &outputChannelInit);
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);
}
void InitializeLED()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // Enable pin 6 = D1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // in alternate function mode
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // @ 50MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // Push-pull
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // No pull-up resistor
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void setup() {
  InitializeLED();
  InitializePWM();
}

void loop() {
  if (Particle.connected() == false) {
    Particle.connect();
  }
  Particle.process();
  int period = 500;
  for (float arg = 0;;arg += 0.0001F)
  {
      uint32_t pulseWidth = (period / 2) * (1 + sin(arg));
      TIM_SetCompare1(TIM4, pulseWidth);
  }
}
