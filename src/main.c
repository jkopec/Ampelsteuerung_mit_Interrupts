/**
 ******************************************************************************
 * @file		main.c
 * @author		JakubKopec
 * @version		1.0
 * @date		2015-11-26
 * @brief		Startet die Ampelsteuerung
 *
 * @bug		Bislang keine Fehler bekannt.
 ******************************************************************************
 */
//inkludieren der gegebenen .h-Files
#include "stm32f3xx.h"
#include "stm32f3_discovery.h"
//inkludieren des selbst geschriebenen .h-Files
#include "state_machine.h"

void EXTI0_Config();

ampelsteuerung_zustand ampel; //!< Variable für den Zustand der Ampel definieren
ampelsteuerung_zustand* licht;

/**
 * Die Main-Function initialisiert den STM + LEDs und führt im
 * Endeffekt die Ampelsteuerung aus.
 *
 * @retval void None
 */
int main(void) {
	//STM initialisieren
	SystemInit();

	//bestimmen wie schnell das Programm ablaufen soll
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);

	//LEDs initialisieren
	BSP_LED_Init(LED_RED);
	BSP_LED_Init(LED_ORANGE);
	BSP_LED_Init(LED_GREEN_2);

	licht = &ampel;

	EXTI0_Config();

	//Anfangsstatus definieren
	licht->status = ROT;
	licht->nachtschaltung = false;

	return EXIT_SUCCESS;
}

// configure interrupts
void EXTI0_Config(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOA clock */
	__GPIOA_CLK_ENABLE();

	/* Configure User Button, connected to PE6 IOs in External Interrupt Mode with Rising edge trigger detection. */
	GPIO_InitStructure.Pin = GPIO_PIN_0;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Enable and set EXTI0 Interrupt to the lowest priority */
	HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

// interrupt callback
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	//wird aufgerufen wenn der User-Button gedrueckt wurde
	if (GPIO_Pin == USER_BUTTON_PIN) {
		licht->nachtschaltung = true;
		/*ampel.unterbrochen = true;
		if (ampel.unterbrochen) {
			//Wenn die Nachtschaltung bereits aktiviert ist
			if (ampel.status == GELB_BLINKEND) {
				licht->nachtschaltung = false;
			} else {
				//Nachtschaltung aktivieren
				licht->nachtschaltung = true;
			}
			ampel.unterbrochen = false;
		}*/
	}
}

//Das ist der Timerinterrupt der jede millisekunde aufgerufen wird
void HAL_SYSTICK_Callback(void) {
	ampelsteuerung(licht);
}

