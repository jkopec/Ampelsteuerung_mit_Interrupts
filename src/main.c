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
ampelsteuerung_zustand* licht; //!< Variable für den Zustand der Ampel definieren

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

/**
  * @brief  Diese Funktion konfiguriert den Interrupt.
  * @param  None
  * @retval None
  */
void EXTI0_Config(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	__GPIOA_CLK_ENABLE(); // GPIOA Uhr aktivieren

	GPIO_InitStructure.Pin = GPIO_PIN_0; // Pin setzen
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING; //Modus setzen -> Rising edge
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0); //auf niedrigste priorität setzen
	HAL_NVIC_EnableIRQ(EXTI0_IRQn); // Interrupt aktivieren
}

/**
  * @brief  Diese Funktion wird nach dem Auftreten des Interrupts als Callback aufgerufen.
  * @param  uint16_t GPIO_Pin
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	//wird aufgerufen wenn der User-Button gedrueckt wurde
	if (GPIO_Pin == USER_BUTTON_PIN) {
		//Wenn die Nachtschaltung bereits aktiviert ist
		if (ampel.status == GELB_BLINKEND) {
			licht->nachtschaltung = false;
			licht->status = ROT;
		} else {
			//Nachtschaltung aktivieren
			licht->nachtschaltung = true;
		}
	}
}

/**
  * @brief  Diese Funktion ist der Timerinterrupt der jede Millisekunde aufgerufen wird.
  * @param  None
  * @retval None
  */
void HAL_SYSTICK_Callback(void) {
	ampelsteuerung(licht);
}

