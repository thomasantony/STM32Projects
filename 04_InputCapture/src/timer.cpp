#include "timer.h"
#include <map>


struct TimerConfig {
    // TIM_TypeDef *timer;
    uint16_t gpio_af;  // Alternative Function
    uint16_t rcc_apb;
    uint32_t rcc_apb_periph;
    uint16_t clock_divider;
    uint16_t cc_irq;   // # of Capture/Compare IRQ
    hal_irq_t irq_by_channel[4];
};

static const uint32_t num_timer_configs = 6;
static const std::map<TIM_TypeDef*, TimerConfig> timer_config = {
    {TIM1, {GPIO_AF_TIM1, 2, RCC_APB2Periph_TIM1, 1, TIM1_CC_IRQn, {SysInterrupt_TIM1_Compare1, SysInterrupt_TIM1_Compare2, SysInterrupt_TIM1_Compare3, SysInterrupt_TIM1_Compare4}}},
    {TIM2, {GPIO_AF_TIM2, 1, RCC_APB1Periph_TIM2, 2, TIM2_IRQn,    {SysInterrupt_TIM2_Compare1, SysInterrupt_TIM2_Compare2, SysInterrupt_TIM2_Compare3, SysInterrupt_TIM2_Compare4}}},
    {TIM3, {GPIO_AF_TIM3, 1, RCC_APB1Periph_TIM3, 2, TIM3_IRQn,    {SysInterrupt_TIM3_Compare1, SysInterrupt_TIM3_Compare2, SysInterrupt_TIM3_Compare3, SysInterrupt_TIM3_Compare4}}},
    {TIM4, {GPIO_AF_TIM4, 1, RCC_APB1Periph_TIM4, 2, TIM4_IRQn,    {SysInterrupt_TIM4_Compare1, SysInterrupt_TIM4_Compare2, SysInterrupt_TIM4_Compare3, SysInterrupt_TIM4_Compare4}}},
    {TIM5, {GPIO_AF_TIM5, 1, RCC_APB1Periph_TIM5, 2, TIM5_IRQn,    {SysInterrupt_TIM5_Compare1, SysInterrupt_TIM5_Compare2, SysInterrupt_TIM5_Compare3, SysInterrupt_TIM5_Compare4}}},
    {TIM8, {GPIO_AF_TIM8, 2, RCC_APB2Periph_TIM8, 1, TIM8_CC_IRQn, {SysInterrupt_TIM8_Compare1, SysInterrupt_TIM8_Compare2, SysInterrupt_TIM8_Compare3, SysInterrupt_TIM8_Compare4}}},
};

struct TimerChannelConfig {
    uint16_t channel;
    uint16_t paired_channel;
    uint16_t it_flag; // TIM_IT_CC1
};

static const uint32_t num_timer_channels = 4;
static const TimerChannelConfig timer_channel_config[num_timer_channels] = {
    {TIM_Channel_1, TIM_Channel_2, TIM_IT_CC1},
    {TIM_Channel_2, TIM_Channel_1, TIM_IT_CC2},
    {TIM_Channel_3, TIM_Channel_4, TIM_IT_CC3},
    {TIM_Channel_4, TIM_Channel_3, TIM_IT_CC4},
};

// Track usage of channel pairs (we use 2 channels per pin to get timing of start and end of the pulse)
static uint8_t channel_pair_used_by_pin[num_timer_configs][num_timer_channels/2] = {0};
