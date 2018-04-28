#include "application.h"
#include <stm32f2xx.h>
#include <stm32f2xx_gpio.h>

SYSTEM_MODE(MANUAL);

// Board LED is on Port PA13 = D7
// Board LED is on Port PB7 = D0
void setup() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enable port B

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; // Enable pin 7
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; // in Output mode
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // @ 2MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // Push-pull
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // No pull-up resistor
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // GPIOA->MODER
    // GPIOA->PUPDR
    // GPIOA->OSPEEDR
    // GPIOA->OTYPER
}

void loop() {
  if (Particle.connected() == false) {
    Particle.connect();
  }

  Particle.process();
  GPIO_SetBits(GPIOB, GPIO_Pin_7);
  // digitalWrite(LED, HIGH);          // sets the LED on
  delay(1000);                       // waits for 200mS
  // digitalWrite(LED, LOW);           // sets the LED off
  GPIO_ResetBits(GPIOB, GPIO_Pin_7);
  delay(2000);                       // waits for 200mS
}
