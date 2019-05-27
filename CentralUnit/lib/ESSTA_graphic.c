#include "ESSTA_graphic.h"

void graphic_init(){
	/*Initialize the LCD and touhscreen*/
	STM32f4_Discovery_LCD_Init();
	#ifdef DEBUG_LOG
		LCD_LOG_Init();
		LCD_LOG_SetHeader("ESSTA System running");
		LCD_LOG_SetFooter("Powered by Erika");
	#else
		IOE_Config();
		InitTouch(-0.09, 0.0656, -353, 10);
		DrawInit(Screen_objects);

		page = HOME_PAGE;

		display_data.temperature = 12.34;
		display_data.humidity = 56.78;
		display_data.eco = true;
		display_data.error = true;
		graphic_update();
	#endif
}

void assign_room_data(struct room_struct room){
	display_data.temperature = room.temperature.value;
	display_data.humidity = room.humidity.value;
	display_data.eco = room.eco;
	display_data.error = room.net_par.error;
}

void graphic_step(){
	switch(page){
		case HOME_PAGE:
			assign_room_data(rooms[0]);
			break;
		case ROOM1_PAGE:
			assign_room_data(rooms[0]);
			break;
		case ROOM2_PAGE:
			assign_room_data(rooms[1]);
			break;
		case ROOM1_SETTINGS_PAGE:
			break;
		case ROOM2_SETTINGS_PAGE:
			break;
		default:
			break;
	}

	#ifdef DEBUG_LOG
		print_rooms();
	#else
		graphic_update();
	#endif
}


void graphic_update(){
	char str[5];
	WClear(&Screen_objects[T_TEMPERATURE]);
	WClear(&Screen_objects[T_HUMIDITY]);
	sprintf(str, "%2.2f", display_data.temperature);
	WPrint(&Screen_objects[T_TEMPERATURE], str);
	sprintf(str, "%2.2f", display_data.humidity);
	WPrint(&Screen_objects[T_HUMIDITY], str);
	if(display_data.eco){
		DrawOn(&Screen_objects[B_ECO]);
	} else {
		DrawOff(&Screen_objects[B_ECO]);
	}
	if(display_data.error){
		DrawOff(&Screen_objects[I_ALERT]);
	}
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

}

void print_rooms(){
	uint8_t i = 0;
	for(i=0; i<MAX_ROOMS; i++){
		log_room(rooms[i]);
	}
}
#endif