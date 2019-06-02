#include "ESSTA_graphic.h"

void touch_event_step(){
	unsigned int px, py;
	TPoint p;
	if (GetTouch_SC_Async(&px, &py)) {
		p.x = px;
		p.y = py;
		OnTouch(Screen_objects, &p);
	}	
}

enum Signal check_Event(){
	static enum Signal last_Sig = NULL_SIG;
	enum Signal sig = NULL_SIG, ret = NULL_SIG;

	if (IsEvent(E_LEFT_ARROW)) sig = LEFT_ARROW_SIG;
	if (IsEvent(E_RIGHT_ARROW)) sig = RIGHT_ARROW_SIG;
	if (IsEvent(E_ECO)) sig = ECO_SIG;
	if (IsEvent(E_SETTINGS)) sig = SETTINGS_SIG;
	if (IsEvent(E_PLUS)) sig = PLUS_SIG;
	if (IsEvent(E_MINUS)) sig = MINUS_SIG;
	ClearEvents();

	// Either

	// Rising
	if(sig != last_Sig){
		switch(sig){
			case LEFT_ARROW_SIG:
				if(last_Sig != LEFT_ARROW_SIG) ret = LEFT_ARROW_SIG;
				break;
			case RIGHT_ARROW_SIG:
				if(last_Sig != RIGHT_ARROW_SIG) ret = RIGHT_ARROW_SIG;
				break;
			case PLUS_SIG:
				if(last_Sig != PLUS_SIG) ret = PLUS_SIG;
				break;
			case MINUS_SIG:
				if(last_Sig != MINUS_SIG) ret = MINUS_SIG;
				break;
			case ECO_SIG:
				if(last_Sig != ECO_SIG) ret = ECO_SIG;
				break;
			case SETTINGS_SIG:
				if(last_Sig != SETTINGS_SIG) ret = SETTINGS_SIG;
				break;
			default:
				break;
		}
		last_Sig = ret;
	}
	return ret;
}

void graphic_init(){
	/*Initialize the LCD and touhscreen*/
	STM32f4_Discovery_LCD_Init();
	#ifdef DEBUG_LOG
		LCD_LOG_Init();
		LCD_LOG_SetHeader("ESSTA System running");
		LCD_LOG_SetFooter("Powered by Erika");
	#else
		IOE_Config();
		InitTouch(-0.088, 0.062775, -343, 10);
		DrawInit(Screen_objects);

		room_selector = 0;
		display_data.temperature.value = 00.00;
		display_data.temperature.format = 'C';
		display_data.humidity.value = 00.00;
		display_data.humidity.format = '%';
		display_data.valve.value = 0;
		display_data.valve.format = '%';
		display_data.eco = false;
		display_data.error = false;
		display_data.id = 255;
		graphic_update(true);

		State_Control = HOME;
		graphic_dispatch(ENTRY_SIG);
	#endif
}

void graphic_step(){
	enum Signal sig;
	#ifdef DEBUG_LOG
		print_rooms();
	#else
		sig = check_Event();
		graphic_dispatch(sig);
		graphic_update(false);
	#endif
}

void graphic_tran(enum State_Control target){
	graphic_dispatch(EXIT_SIG);
	State_Control = target;
	graphic_dispatch(ENTRY_SIG);
}

void graphic_dispatch(enum Signal sig){
	switch(State_Control){
		case HOME:
			manage_home_page(sig);
			break;
		case HOME_SETTINGS:
			manage_home_settings(sig);
			break;
		case ROOM:
			manage_room_page(sig);
			break;
		default:
			break;
	}
}

void assign_room_data(struct room_struct room){
	if(room.id != 255 ){
		display_data.id = room.id;
	}
	display_data.temperature = room.temperature;
	display_data.humidity = room.humidity;
	display_data.valve = room.valve;
	display_data.eco = room.eco;
	display_data.error = room.net_par.error;
	if(room.temperature.value >= (home_struct.des_temperature - TEMPERATURE_GOAL_OFFSET)){
		DrawOn(&Screen_objects[I_HOT_TEMPERATURE]);
	} else {
		DrawOn(&Screen_objects[I_COLD_TEMPERATURE]);
	}
}

void assign_home_data(){
	display_data.id = 255;	// special id
	display_data.temperature = home_struct.temperature;
	display_data.humidity = home_struct.humidity;
	display_data.valve = home_struct.valve;
	display_data.eco = home_struct.eco;
	display_data.error = home_struct.error;
	if(home_struct.temperature.value >= (home_struct.des_temperature - TEMPERATURE_GOAL_OFFSET)){
		DrawOn(&Screen_objects[I_HOT_TEMPERATURE]);
	} else {
		DrawOn(&Screen_objects[I_COLD_TEMPERATURE]);
	}
}

void assign_home_settings_data(){
	display_data.id = 255;	// special id
	display_data.temperature.value = home_struct.des_temperature;
	display_data.temperature.format = 'C';
	display_data.humidity = home_struct.humidity;
	display_data.valve = home_struct.valve;
	display_data.eco = home_struct.eco;
	display_data.error = home_struct.error;	
	DrawOn(&Screen_objects[I_HOT_TEMPERATURE]);
}

void increase_des_temperature(){
	if(home_struct.des_temperature < TEMP_MAX){
		home_struct.des_temperature += STEP_TEMPERATURE;
	}
}

void decrease_des_temperature(){
	if(home_struct.des_temperature > TEMP_MIN){
		home_struct.des_temperature -= STEP_TEMPERATURE;
	}
}

void manage_home_page(enum Signal sig){
	assign_home_data();
	switch(sig){
		case ENTRY_SIG:
			WClear(&Screen_objects[B_PLUS]);
			WClear(&Screen_objects[B_MINUS]);
			WClear(&Screen_objects[T_ROOM_ID]);
			DrawOn(&Screen_objects[I_HOME]);
			break;
		case EXIT_SIG:
			break;
		case LEFT_ARROW_SIG:
			if(rooms[N_ROOMS-1].id != 255){
				room_selector = N_ROOMS-1;
				graphic_tran(ROOM);
			}
			break;
		case RIGHT_ARROW_SIG:
			if(rooms[0].id != 255){
				room_selector = 0;
				graphic_tran(ROOM);
			}
			break;
		case SETTINGS_SIG:
			graphic_tran(HOME_SETTINGS);
			break;
		case ECO_SIG:
			break;
		case PLUS_SIG:
			break;
		case MINUS_SIG:
			break;
		default:
			break;
	}
}

void manage_home_settings(enum Signal sig){
	assign_home_settings_data();
	DrawOff(&Screen_objects[B_MINUS]);
	DrawOff(&Screen_objects[B_PLUS]);
	switch(sig){
		case ENTRY_SIG:
			WClear(&Screen_objects[T_ROOM_ID]);
			DrawOn(&Screen_objects[I_HOME]);
			break;
		case EXIT_SIG:
			// assign new desired temperature
			break;
		case LEFT_ARROW_SIG:
			if(rooms[N_ROOMS-1].id != 255){
				room_selector = N_ROOMS-1;
				graphic_tran(ROOM);
			}
			break;
		case RIGHT_ARROW_SIG:
			if(rooms[0].id != 255){
				room_selector = 0;
				graphic_tran(ROOM);
			}
			break;
		case SETTINGS_SIG:
			graphic_tran(HOME);
			break;
		case ECO_SIG:
			// enable/disable eco mode
			break;
		case PLUS_SIG:
			DrawOn(&Screen_objects[B_PLUS]);
			increase_des_temperature();
			break;
		case MINUS_SIG:
			DrawOn(&Screen_objects[B_MINUS]);
			decrease_des_temperature();
			break;
		default:
			break;
	}
}

void manage_room_page(enum Signal sig){
	assign_room_data(rooms[room_selector]);
	switch(sig){
		case ENTRY_SIG:
			WClear(&Screen_objects[B_PLUS]);
			WClear(&Screen_objects[B_MINUS]);
			DrawOn(&Screen_objects[I_ROOM]);
			break;
		case EXIT_SIG:
			break;
		case LEFT_ARROW_SIG:
			if(room_selector > 0){
				if(rooms[room_selector - 1].id != 255){
					room_selector--;
					graphic_tran(ROOM);
				}
			} else {
				graphic_tran(HOME);
			}
			break;
		case RIGHT_ARROW_SIG:
			if(room_selector < (N_ROOMS - 1)){
				if(rooms[room_selector + 1].id != 255){
					room_selector++;
					graphic_tran(ROOM);
				}
			} else {
				graphic_tran(HOME);
			}
			break;
		case SETTINGS_SIG:
			graphic_tran(HOME_SETTINGS);
			break;
		case ECO_SIG:
			break;
		case PLUS_SIG:
			break;
		case MINUS_SIG:
			break;
		default:
			break;
	}
}

void graphic_update(bool init){
	char str[5];
	if((display_data.temperature.value != old_display_data.temperature.value) || init){
		WClear(&Screen_objects[T_TEMPERATURE]);
		sprintf(str, "%2.2f%c", display_data.temperature.value, display_data.temperature.format);
		WPrint(&Screen_objects[T_TEMPERATURE], str);
	}
	if((display_data.humidity.value != old_display_data.humidity.value) || init){
		WClear(&Screen_objects[T_HUMIDITY]);
		sprintf(str, "%2.2f%c", display_data.humidity.value, display_data.humidity.format);
		WPrint(&Screen_objects[T_HUMIDITY], str);
	}

	if((display_data.valve.value != old_display_data.valve.value) || init){
		WClear(&Screen_objects[T_VALVE]);
		sprintf(str, "%3d%c", display_data.valve.value, display_data.valve.format);
		WPrint(&Screen_objects[T_VALVE], str);
	}

	if(display_data.id != 255){
		if((display_data.id != old_display_data.id) || init){
			WClear(&Screen_objects[T_ROOM_ID]);
			sprintf(str, "%1d", display_data.id);
			WPrint(&Screen_objects[T_ROOM_ID], str);
		}
	}

	if((display_data.error != old_display_data.error) || init){
		if(display_data.error){
			DrawOn(&Screen_objects[I_WARNING]);
		} else {
			DrawOff(&Screen_objects[I_WARNING]);
		}
	}

	if((display_data.eco != old_display_data.eco) || init){
		if(display_data.eco){
			DrawOn(&Screen_objects[B_ECO]);
		} else {
			DrawOff(&Screen_objects[B_ECO]);
		}
	}
	old_display_data = display_data;
}


//-----------------------------------
#ifdef DEBUG_LOG
void log_room(struct room_struct my_room){
	char tmp[16];
	LCD_UsrLog("\r\n-----------------------");
	memset(tmp, '\0', 16);
	sprintf(tmp, "\r\n Id:%d", my_room.id);
	LCD_UsrLog(tmp);

	memset(tmp, '\0', 16);
	sprintf(tmp, "\r\n eco:%d", my_room.eco);
	LCD_UsrLog(tmp);

	memset(tmp, '\0', 16);
	sprintf(tmp, "\r\n Temp:%2.2f %c", my_room.temperature.value, my_room.temperature.format);
	LCD_UsrLog(tmp);

	memset(tmp, '\0', 16);
	sprintf(tmp, "\r\n Hum:%2.2f %c", my_room.humidity.value, my_room.humidity.format);
	LCD_UsrLog(tmp);

	memset(tmp, '\0', 16);
	sprintf(tmp, "\r\n Valve:%03d %c", my_room.valve.value, my_room.valve.format);
	LCD_UsrLog(tmp);

}

void print_rooms(){
	uint8_t i = 0;
	for(i=0; i<N_ROOMS; i++){
		log_room(rooms[i]);
	}
}
#endif