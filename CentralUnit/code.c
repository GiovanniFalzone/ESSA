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

#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_lcd.h"
#include "lcd_log.h"
#include <stdio.h>
#include <string.h>

#include "ESSTA_lib.h"
#include "ESSTA_JSON_lib.h"

#define DEBUG_LOG_MODE

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

void check_USART_RX() {
	EE_UINT8 ch;
	while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_RXNE) != RESET){
		ch = USART_ReceiveData(EVAL_COM1);
		if(ch != '\n' && ch != '\r'){
			if(msg_pos >= MSG_LEN){
				LCD_ErrLog("\r\nMessage too long or interleaved %d", msg_pos);
				msg_pos = 0;
				memset(msg, '\0', MSG_LEN);
			}
			msg[msg_pos++] = ch;
		} else {
			ActivateTask(CheckMessage);
		}
	}
}

void send_char(char c) {
	while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET);	// wait finished
	USART_SendData(EVAL_COM1, (EE_UINT8) c);
	while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET);	// wait finished
}

void send_string(char* str) {
	EE_UINT8 i = 0;
	LCD_UsrLog("\r\nSend: ");
	LCD_UsrLog(str);
	while (str[i] != '\0') {
		send_char(str[i++]);
	}
}

void print_string(char* str){
	LCD_UsrLog("\r\n");
	LCD_UsrLog(str);
}


TASK(CheckMessage) {
	switch(check_message(msg)){
		case 1:	// message correct JSON
			msg_pos = 0;
			uint8_t id = JSON_to_room_struct(msg);
			memset(msg, '\0', MSG_LEN);
			if(id != 255){
				rooms[id].net_par.response = true;
				rooms[id].net_par.error = false;
			}
			break;

		case 2:	// message incomplete
			break;

		case 3:	// message corrupted
			#ifdef DEBUG_LOG
				LCD_ErrLog("\r\n not JSON compliant!");
			#endif
			msg_pos = 0;
			memset(msg, '\0', MSG_LEN);
			break;

		default:
			break;
	}
}

TASK(PrintGraphic) {
//	print_rooms();
}

TASK(TaskPollingRooms) {
	static uint8_t id = 0;
	char* msg[32];
	if(rooms[id].net_par.response == true) {
		rooms[id].net_par.response = false;
		rooms[id].net_par.resend = 0;
		id = (id+1)%N_ROOMS;
	} else {
		if(rooms[id].net_par.resend > 2){
			LCD_ErrLog("\r\n Polling error node %2d crashed, Resend:%d", (id+1), rooms[id].net_par.resend);
			rooms[id].net_par.error = true;
			rooms[id].net_par.resend = 0;
			id = (id+1)%N_ROOMS;
		} else {
			rooms[id].net_par.resend++;
		}
	}

	sprintf(msg, "start::{\"Id\":\"%02d\"}\n", (id+1));
	send_string(msg);
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

		/*Initialize the LCD*/
	STM32f4_Discovery_LCD_Init();

	#ifdef DEBUG_LOG
		LCD_LOG_Init();
		LCD_LOG_SetHeader("Hi ... Erika is running!");
		LCD_LOG_SetFooter("Erika RTOS LCD log Demo");
		LCD_UsrLog("\r\nTest-> Log Initialized!");
		LCD_DbgLog("\r\nTest-> DBG message!");
		LCD_ErrLog("\r\nTest-> ERR message!");
	#endif

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


	/* Output message */
	char msg[32];
	memset(msg, '\0', 32);
	sprintf(msg, "Hello Gionni!!!\n\0");
	print_string(msg);
	send_string(msg);


	/* Program cyclic alarms which will fire after an initial offset,
	 * and after that periodically
	 * */

	SetRelAlarm(Alarm_PollingRooms, 1000, 30000);
	SetRelAlarm(Alarm_PrintGraphic, 1000, 60000);

	init_rooms();

	/* Forever loop: background activities (if any) should go here */
	for (;;) {
		check_USART_RX();
	}

}

