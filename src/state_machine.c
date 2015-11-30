/**
 ******************************************************************************
 * @file		state_machine.c
 * @author		JakubKopec
 * @version		1.0
 * @date		2015-11-26
 * @brief		State Centric State Machine with Hidden Transitions
 *
 * @bug		Bislang keine Fehler bekannt.
 ******************************************************************************
 */
#include "state_machine.h"

/**
 * Diese Function repräsentiert die State Centric State Machine
 *
 * Hier wird der genaue Ablauf definiert.
 *
 * @param ampel Der Zustand der Ampel.
 * @retval void None
 */
void ampelsteuerung(ampelsteuerung_zustand* ampel) {
	//je nach aktuellem Status wird der jeweilige "case" ausgeführt

	// counter for timer interrupts
	static int counter = 0;
	// increment counter to be able to know how long we have waited
	counter++;

	switch (ampel->status) {

	case ROT://wenn Status = ROT
		if(ampel->nachtschaltung){
			rote_led_an();//rote LED an
			if(counter>=2000){
				counter=0;
				ampel->status = GELB_BLINKEND;//auf naechsten status setzen
			}
		}else{
			rote_led_an();//rote LED an
			if(counter>=2000){
				counter=0;
				ampel->status = ROT_GELB;//auf naechsten status setzen
			}
		}
		break;

	case ROT_GELB://wenn Status = ROT_GELB
		rote_gelbe_led_an();//rote und gelbe LED an
		if(counter>=2000){
			counter=0;
			ampel->status = GRUEN;//auf naechsten Status setzen
		}
		break;

	case GRUEN://wenn Status = GRUEN
		gruene_led_an();//gruene LED an
		if(counter>=2000){
			counter=0;
			ampel->status = GRUEN_BLINKEND;//auf naechsten status setzen
		}
		break;

	case GRUEN_BLINKEND://wenn Status = GRUEN_BLINKEND
		if(counter<=500){
			led_reset();
		}else if(counter<=1000){
			gruene_led_an();
		}else if(counter<=1500){
			led_reset();
		}else if(counter<=2000){
			gruene_led_an();
		}else if(counter<=2500){
			led_reset();
		}else if(counter<=3000){
			gruene_led_an();
		}else if(counter<=3500){
			led_reset();
			counter = 0;
			ampel->status = GELB;
		}
		break;

	case GELB://wenn Status = GELB
		gelbe_led_an();//gelbe LED an
		if(counter>=2000){
			counter=0;
			ampel->status = ROT;//auf naechsten Status setzen
		}
		break;

	case GELB_BLINKEND://wenn Status = GELB_BLINKEND
		if(counter<=500){
			led_reset();
			gelbe_led_an();
		}else if(counter<=1000){
			led_reset();
		}else if(counter<=1500){
			gelbe_led_an();
		}else if(counter<=2000){
			led_reset();
			counter = 0;
			ampel ->status = GELB_BLINKEND;
		}
		break;

	default://wenn kein Status gesetzt ist
		ampel->status = ROT;//Status auf ROT setzen
		break;
	}
}
