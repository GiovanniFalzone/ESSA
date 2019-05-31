#include "ESSTA_lib.h"

struct room_struct rooms[N_ROOMS] = {0,0,{0,0},{0,0},{0,0},{0,0,0}};
struct home_struct home_struct = {0,0,0,{0,0},{0,0},{0,0}};

void init_home(){
	home_struct.temperature.value = 0.0;
	home_struct.temperature.format = 'C';
	home_struct.humidity.value = 0.0;
	home_struct.humidity.format = '%';
	home_struct.des_temperature = DEFAULT_DES_TEMPERATURE;
	home_struct.eco = false;
	home_struct.error = false;
}

void init_rooms(){
	uint8_t i = 0;
	for(i=0;i<N_ROOMS;i++){
		rooms[i].id=255;
		rooms[i].eco=0;
		rooms[i].temperature.value=0.0;
		rooms[i].temperature.format='?';
		rooms[i].humidity.value=0.0;
		rooms[i].humidity.format='?';
		rooms[i].net_par.response = false;
		rooms[i].net_par.resend = 0;
		rooms[i].net_par.error = false;
	}
}

void send_char(char c) {
	while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET);	// wait finished
	USART_SendData(EVAL_COM1, (uint8_t) c);
	while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET);	// wait finished
}

void send_string(char* str) {
	uint8_t i = 0;
	#ifdef DEBUG_LOG
		LCD_UsrLog("\r\nSend: ");
		LCD_UsrLog(str);
	#endif
	while (str[i] != '\0') {
		send_char(str[i++]);
	}
}

void ESSTA_send_node_request(uint8_t id){
	char* msg[50];
	sprintf(msg, "start::{\"Id\":\"%02d\",\"DTemp\":\"%2.2f\"}\n\0", id, home_struct.des_temperature);
	send_string(msg);
}

void ESSTA_init(){
	graphic_init();
	send_string("ESSTA System running\n\0");
	init_home();
	init_rooms();
}

void ESSTA_compute_house_status(){
	uint8_t i = 0, count=0, valv=0;
	float temp = 0, hum = 0;
	bool eco = false, err = false;

	for(i=0; i<N_ROOMS; i++){
		if(rooms[i].id != 255){
			count++;
			if(rooms[i].temperature.format=='C'){
				temp += rooms[i].temperature.value;
			} else {	// conversion
				;
			}

			if(rooms[i].humidity.format=='%'){
				hum += rooms[i].humidity.value;
			} else {	// conversion
				;
			}

			if(rooms[i].valve.format=='%'){
				valv += rooms[i].valve.value;
			} else {	// conversion
				;
			}
			eco |= rooms[i].eco;
			err |= rooms[i].net_par.error;
		}
	}
	home_struct.eco = eco;
	home_struct.error = err;
	if(temp > 0 && count > 0){
		home_struct.temperature.value = temp/count;
	}
	if(hum > 0 && count > 0){
		home_struct.humidity.value = hum/count;
	}
	home_struct.valve.value = (valv+1)/count;	// round to next digit
	home_struct.temperature.format = 'C';
	home_struct.humidity.format = '%';
	home_struct.valve.format = '%';
}
