/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2013  Evidence Srl
 *
 * This file is part of ERIKA Enterprise.
 *
 * ERIKA Enterprise is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation,
 * (with a special exception described below).
 *
 * Linking this code statically or dynamically with other modules is
 * making a combined work based on this code.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this code with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this code, you may extend
 * this exception to your version of the code, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 *
 * ERIKA Enterprise is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with ERIKA Enterprise; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 * ###*E*### */

/*
 * Simple demo that shows how to use the USART peripheral.
 *
 * Author: 2013  Gianluca Franchino.
 *
 */


#include "ee.h"
#include "ee_irq.h"

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_lcd.h"
#include "lcd_log.h"

#include <stdio.h>
#include <string.h>

#include "lib/ESSTA_definitions.h"
#include "lib/ESSTA_lib.h"
#include "lib/ESSTA_JSON_lib.h"
#include "lib/ESSTA_graphic.h"

USART_InitTypeDef USART_InitStructure;

uint8_t msg_pos=0;
char msg[MSG_LEN];


/*
 * SysTick ISR2
 */
ISR2(systick_handler) {
	/* count the interrupts, waking up expired alarms */
	CounterTick(myCounter);
}

 ISR2(usart6_rx_handler) {
 	STM_EVAL_LEDToggle(LED4);
 	send_string("ciao\0");
 }

TASK(Task_LCD_Graphic) {		// 50 ms
	touch_event_step();
	graphic_step();
}

void check_USART_RX() {
	EE_UINT8 ch;
	while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_RXNE) != RESET){
		ch = USART_ReceiveData(EVAL_COM1);
		if(ch != '\n' && ch != '\r'){
			if(ch == ' ') continue;
			if(msg_pos >= MSG_LEN){
				#ifdef DEBUG_LOG
					LCD_ErrLog("\r\nMessage too long or interleaved %d", msg_pos);
				#endif
				msg_pos = 0;
				memset(msg, '\0', MSG_LEN);
			}
			msg[msg_pos++] = ch;
		} else {
			ActivateTask(CheckMessage);
		}
	}
}

// sporadic task
TASK(CheckMessage) {
	switch(check_message(msg)){
		case 1:	// message correct JSON
			msg_pos = 0;
			struct room_struct my_room = JSON_to_room_struct(msg);
			memset(msg, '\0', MSG_LEN);
			if(my_room.id != 255){
				rooms[my_room.id-1] = my_room;
				rooms[my_room.id-1].net_par.response = true;
				rooms[my_room.id-1].net_par.error = false;
			}
			break;

		case 2:	// message incomplete
			break;

		case 3:	// message corrupted
			#ifdef DEBUG_LOG
				LCD_ErrLog("\r\n not JSON compliant!");
				LCD_ErrLog("\r\n msg: %s", msg);
				#endif
			msg_pos = 0;
			memset(msg, '\0', MSG_LEN);
			break;

		default:
			break;
	}
}

TASK(TaskPollingRooms) {
	static uint8_t id = 0;
	if(rooms[id].net_par.response == true) {
		rooms[id].net_par.response = false;
		rooms[id].net_par.resend = 0;
		id = (id+1)%N_ROOMS;
	} else {
		if(rooms[id].net_par.resend > 2){
			#ifdef DEBUG_LOG
				LCD_ErrLog("\r\n Polling error node %2d crashed", (id+1));
			#endif
			rooms[id].net_par.error = true;
			rooms[id].net_par.resend = 0;
			id = (id+1)%N_ROOMS;
		} else {
			rooms[id].net_par.resend++;
		}
	}
	ESSTA_send_node_request(id+1);
	ESSTA_compute_house_status();
}

TASK(UserTask) {
	;
}

int main(void) {
	/*
	 * Setup the microcontroller system.
	 * Initialize the Embedded Flash Interface, the PLL and update the
	 * SystemFrequency variable.
	 * For default settings look at:
	 * pkg/mcu/st_stm32_stm32f4xx/src/system_stm32f4xx.c
	 */
	SystemInit();

	/*Initialize Erika related stuffs*/
	EE_system_init();

	/*Initialize systick */
	EE_systick_set_period(MILLISECONDS_TO_TICKS(1, SystemCoreClock));
	EE_systick_enable_int();
	EE_systick_start();

	STM_EVAL_LEDInit(LED4);

	/* USARTx configured as follow:
		- BaudRate = 115200 baud
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	STM_EVAL_COMInit(COM1, &USART_InitStructure);

	ESSTA_init();

	/* Program cyclic alarms which will fire after an initial offset,
	 * and after that periodically
	 * */
	#ifdef DEBUG_LOG
		SetRelAlarm(Alarm_LCD_Graphic, 1000, 30000);
	#else
		SetRelAlarm(Alarm_LCD_Graphic, 1000, 100);
	#endif
	SetRelAlarm(Alarm_PollingRooms, 1000, 5000);

	/* Forever loop: background activities (if any) should go here */
	for (;;) {
		check_USART_RX();
	}

}

