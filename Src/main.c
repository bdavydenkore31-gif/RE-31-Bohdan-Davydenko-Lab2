#include <stdint.h>
#include "stm32f411xe.h"

void PWM_Init (void);

int main(void)
{
    /*Світлодіод підлючений до PA5(зелений на платі) та PA1 (синій)
     *  та PA9(червноний)  кнопка до PC 13 (підтяжка до 3 В)*/

	/*Вмикаємо тактування*/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	/*Визначаємо режим роботи PA5*/

	GPIOA->MODER &= ~GPIO_MODER_MODER5;       // Скидання
	GPIOA->MODER &= ~GPIO_MODER_MODER5_0; //PA5 встановлюємо як AF
	GPIOA->MODER |= GPIO_MODER_MODER5_1;

	/*Визначаємо режим роботи PA3*/
	GPIOA->MODER &= ~GPIO_MODER_MODER1;       // Скидання
	GPIOA->MODER &= ~GPIO_MODER_MODER1_0; //PA1 встановлюємо як AF
	GPIOA->MODER |= GPIO_MODER_MODER1_1;


	/*Визначаємо режим роботи PA9*/
	GPIOA->MODER &= ~GPIO_MODER_MODER9;       // Скидання
	GPIOA->MODER |= GPIO_MODER_MODER9_0; //PA9 встановлюємо як вихід



	GPIOC->MODER &= ~GPIO_MODER_MODER13;//PC на вхід


	//ARL від 0 до 8
	// Налаштування PA5 (Low Register: AFR[0]) ---
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL5;
	GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFSEL5_Pos); // // Записати 1 (AF1)

	// Налаштування PA3 (High Register: AFR[1]) ---
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL1;
	GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFSEL1_Pos); // // Записати 1 (AF1)

	PWM_Init();

	while (1) {

		if (!(GPIOC->IDR & GPIO_IDR_IDR_13)) { //коли натинута 0 бо підтяжка на +
		        // Вмикаємо світлодіод на PA9
		        GPIOA->ODR |= GPIO_ODR_ODR_9;
		}else {
		   // Вимикаємо світлодіод на PA9
		     GPIOA->ODR &= ~GPIO_ODR_ODR_9;
		 }
	}

}

void PWM_Init (){


	//Нам потрібен таймер 2 для PA5
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // APB1 Timer Clock = 16 MHz.
    // PSC = (16000000 / 10000) - 1 = 1599.
    TIM2->PSC = 1599;

    // Частота 10000 Гц. Для 10 Гц  ARR = 10000/10 - 1 = 999.
    TIM2->ARR = 999; //10Гц

    TIM2->CCR1 = 100;//10 мс
    TIM2->CCR2 = 100;//10 мс


	// Встановлюємо PWM Mode 1 (110) .
	 TIM2->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2); // CH1
	 TIM2->CCMR1 |= (TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2); // CH1

	/*Вмикаємо канал*/
	 TIM2->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC2E) ;
	 TIM2->CR1 |= TIM_CR1_CEN;

	 //Зміна частоти тільки коли дорахувал цикл
	 TIM2->CR1 |= TIM_CR1_ARPE;


}


void SystemClock_Config_HSE_Bypass(void) {
    // 1. Вмикаємо режим BYPASS
    RCC->CR |= RCC_CR_HSEBYP;

    // 2. Вмикаємо HSE
    RCC->CR |= RCC_CR_HSEON;

    // 3. Очікуємо, поки не встановиться прапорець готовності HSE
    while (!(RCC->CR & RCC_CR_HSERDY)) {

    }


    // 4. Перемикаємо джерело системного тактування (SYSCLK) на HSE
    RCC->CFGR &= ~RCC_CFGR_SW;      // Очищаємо біти SW
    RCC->CFGR |= RCC_CFGR_SW_HSE;   // Встановлюємо HSE як джерело

    // 5. Очікуємо, поки HSE не стане активним джерелом SYSCLK
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE) {
    }
}
