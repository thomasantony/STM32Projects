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

uint32_t channel_idx = 0;

const int NUM_PULSES = 10;
volatile uint16_t pulse_list[NUM_PULSES] = {0,};
volatile int last_pulse_idx = 0;

void TIM3_Interrupt_Handler_override()
{
  uint16_t pulse_stop  = (&TIM3->CCR1)[0];
  uint16_t pulse_start = (&TIM3->CCR1)[1];
  // We assume that the pulse length is within one timer period.
  uint16_t pulse_len = pulse_stop - pulse_start;
  if(last_pulse_idx == 1)
  {
    pulse_len = 6969;
  }
  pulse_list[last_pulse_idx % NUM_PULSES] = pulse_len;
  last_pulse_idx ++;
}


void InitializeCapture()
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  TIM_TimeBaseInitTypeDef timerInitStructure;
  timerInitStructure.TIM_Prescaler = SIT_PRESCALERm;
  timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  timerInitStructure.TIM_Period = 0xFFFF;
  timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV4;
  timerInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3, &timerInitStructure);

  // Channel 1 init
  TIM_ICInitTypeDef inputCaptureInit;
  inputCaptureInit.TIM_ICPolarity = TIM_ICPolarity_Rising;
  inputCaptureInit.TIM_Channel = TIM_Channel_1;
  inputCaptureInit.TIM_ICSelection = TIM_ICSelection_DirectTI;
  inputCaptureInit.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  inputCaptureInit.TIM_ICFilter = 0x0;
  TIM_ICInit(TIM3, &inputCaptureInit);

  // Paired channel init
  inputCaptureInit.TIM_Channel = TIM_Channel_2;
  inputCaptureInit.TIM_ICPolarity = TIM_ICPolarity_Falling;
  inputCaptureInit.TIM_ICSelection = TIM_ICSelection_IndirectTI;
  TIM_ICInit(TIM3, &inputCaptureInit);

  TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE); // Enable interrupt for capture
  TIM_Cmd(TIM3, ENABLE);

  // Synchronize timer counter and current Timestamp
  // TIM_SetCounter(timer_, Timestamp::cur_time().get_raw_value() & 0xFFFF);
}

void EnableTimerInterrupt()
{
    if (!attachSystemInterrupt(SysInterrupt_TIM3_Compare1, TIM3_Interrupt_Handler_override));	//error
    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = TIM3_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
    nvicStructure.NVIC_IRQChannelSubPriority = 0;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicStructure);
}


void InitializeGPIO()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; // Enable PB4 = D3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // in alternate function mode
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // @ 50MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // Push-pull
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // No pull-up resistor
  // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; // Pull down by default
  // GPIO_PuPd_DOWN
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);
}

void InitializePWMSignal()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // Enable pin 6 = D1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // in alternate function mode
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // @ 50MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // Push-pull
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // No pull-up resistor
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  TIM_TimeBaseInitTypeDef timerInitStructure;
  timerInitStructure.TIM_Prescaler = SIT_PRESCALERm;
  timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  timerInitStructure.TIM_Period = 500;
  timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  timerInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM4, &timerInitStructure);
  TIM_Cmd(TIM4, ENABLE);

  TIM_OCInitTypeDef outputChannelInit = {0,};
  outputChannelInit.TIM_OCMode = TIM_OCMode_PWM1;
  outputChannelInit.TIM_Pulse = 301;
  outputChannelInit.TIM_OutputState = TIM_OutputState_Enable;
  outputChannelInit.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM4, &outputChannelInit);
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);
}


void setup() {
  InitializeGPIO();
  InitializePWMSignal();
  EnableTimerInterrupt();
  InitializeCapture();

  // pinMode(D7, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  if (Particle.connected() == false) {
    Particle.connect();
  }
  Particle.process();

  for(int i=0;i<NUM_PULSES;i++)
  {
    Serial.print(pulse_list[i], DEC);
    Serial.print(", ");
  }
  Serial.println();
  delay(1000);
  // digitalWrite(D7, HIGH);
  // delay(500);
  // digitalWrite(D7, LOW);
  // delay(500);
  // int period = 500;
  // for (float arg = 0;;arg += 0.0001F)
  // {
  //     uint32_t pulseWidth = (period / 2) * (1 + sin(arg));
  //     TIM_SetCompare1(TIM3, pulseWidth);
  // }
}
