#include "ESSTA_JSON_lib.h"

bool strequal(const char* s1, const char* s2){
	if(*s1 != '\0' && *s2 != '\0'){
		while(*s1 != '\0' && *s1 == *s2){
			s1++;
			s2++;
		}
		if(*s1=='\0'){
			return true;
		} else {
			return false;
		}
	} else {
		if(*s1 != *s2){
			return false;
		} else {
			return true;	// both empty string
		}
	}
}

uint8_t str_3d_to_uint8(char* str){
	uint8_t ret = 0;

	ret = (str[0] - '0')*100;
	ret += (str[1] - '0')*10;
	ret += (str[2] - '0');

	return ret;
}

uint8_t str_2d_to_uint8(char* str){
	uint8_t ret = (str[0] - '0')*10;
	ret += (str[1] - '0');
	return ret;
}

float str_2_2d_to_float(char *str){	//01.23
	float ret = (float)str_2d_to_uint8(str);
	ret += ((float)str_2d_to_uint8(str+3))/100;
}

uint8_t check_message(char* str){
	uint8_t count_start_graph = 0;
	uint8_t count_end_graph = 0;
	uint8_t count_start_quad = 0;
	uint8_t count_end_quad = 0;
	uint8_t count_quotes = 0;
	uint8_t len = 0;
// {"Id":"01","Eco":"1","sens":[{"Nm":"Tmp","Val":"24.00","Fmt":"C"},{"Nm":"Hum","Val":"56.78","Fmt":"%"}]}
	while(*str != '\0' && len<MSG_LEN){
		switch(*str){
			case '"':
				count_quotes++;
				break;
			case '{':
				count_start_graph++;
				break;
			case '}':
				count_end_graph++;
				break;
			case '[':
				count_start_quad++;
				break;
			case ']':
				count_end_quad++;
				break;
			default:
				break;
		}
		str++;
		len++;
	}
	if(len>=MSG_LEN)
		return 3;	// message corrupted

	if(count_start_graph > count_end_graph)	// message incomplete
		return 2;

	if(count_start_graph==count_end_graph && count_start_quad==count_end_quad && (count_quotes%2)==0)	// message correct
		return 1;

	#ifdef DEBUG_LOG
		LCD_UsrLog("\r\n sgr:%d egr:%d squ:%d equ:%d, quo%d:", count_start_graph, count_end_graph, count_start_quad, count_end_quad, count_quotes);
	#endif
	return 3;	// message corrupted
}

bool set_room_ID(char* str, struct room_struct* my_room){
	bool ret = false;
	uint8_t id = 0;
	char field[3];
	char value[3];

	// {"Id":"01"
	memcpy(field, str+2, 2);

	field[2]='\0';
	uint8_t check = strequal(field, "Id\0");	// strcmp(field, "Id\0");

	if(check){
		memcpy(value, str+7, 2);
		value[3]='\0';
		id = str_2d_to_uint8(value);
		if (id > 0 && id <= N_ROOMS){
			my_room->id = id;
			ret = true;
		} else {
			my_room->id = 255;
			ret = false;
		}
	} else {
		my_room->id = 255;
		ret = false;
	}

	#ifdef DEBUG_LOG
	if(!ret){
		char tmp[16];
		sprintf(tmp, "\r\n %s=%d", field, id);
		LCD_ErrLog(tmp);
	}
	#endif
	return ret;
}

bool set_room_Eco(char* str, struct room_struct* my_room){
	bool ret = false;
	bool value = 0;
	char field[4];

	// {"Id":"01","Eco":"1",
	memcpy(field, str+12, 3);

	field[3]='\0';
	uint8_t check = strequal(field, "Eco\0");	// strcmp(field, "Id\0");

	if(check){
		value = *(str+18) - '0';
		if (value >= 0 && value <= 1){
			my_room->eco = value;
			ret = true;
		} else {
			ret = false;
		}
	} else {
		ret = false;
	}

	#ifdef DEBUG_LOG
	if(!ret){
		char tmp[16];
		sprintf(tmp, "\r\n %s=%d", field, value);
		LCD_ErrLog(tmp);
	}
	#endif
	return ret;
}

bool check_json_sensor(char* str, char* sensor_name){
	// {"Id":"01","Eco":"1","sens":[{"Nm":"Tmp","Val":"24.00","Fmt":"C"},{"Nm":"Hum","Val":"56.78","Fmt":"%"}]}
	// {"Nm":"Tmp","Val":"24.00","Fmt":"C"}
	bool ret = true;
	char tmp[16];

	memcpy(tmp, str+2, 2);
	tmp[2]='\0';
	ret &= strequal(tmp, "Nm\0");

	#ifdef DEBUG_LOG_JSON
		char tmp_deb[32];
		if(!ret){
			sprintf(tmp_deb, "\r\n JSON_S check_Nm %s", tmp);
			LCD_ErrLog(tmp_deb);
		}
	#endif

	memcpy(tmp, str+7, 3);
	tmp[3]='\0';
	ret &= strequal(tmp, sensor_name);

	#ifdef DEBUG_LOG_JSON
		if(!ret){
			sprintf(tmp_deb, "\r\n JSON_S check_Name %s", tmp);
			LCD_ErrLog(tmp_deb);
		}
	#endif

	memcpy(tmp, str+13, 3);
	tmp[3]='\0';
	ret &= strequal(tmp, "Val\0");

	#ifdef DEBUG_LOG_JSON
		if(!ret){
			sprintf(tmp_deb, "\r\n JSON_S check_Val %s", tmp);
			LCD_ErrLog(tmp_deb);
		}
	#endif


	memcpy(tmp, str+27, 3);
	tmp[3]='\0';
	ret &= strequal(tmp, "Fmt\0");

	#ifdef DEBUG_LOG_JSON
		if(!ret){
			sprintf(tmp_deb, "\r\n JSON_S check_Fmt %s", tmp);
			LCD_ErrLog(tmp_deb);
		}
	#endif

	return ret;
}

bool check_json_actuator(char* str, char* sensor_name){
// {"Nm":"Vlv","Val":"057","Fmt":"%"}
	bool ret = true;
	char tmp[16];

	memcpy(tmp, str+2, 2);
	tmp[2]='\0';
	ret &= strequal(tmp, "Nm\0");

	#ifdef DEBUG_LOG_JSON
		char tmp_deb[32];
		if(!ret){
			sprintf(tmp_deb, "\r\n JSON_S check_Nm %s", tmp);
			LCD_ErrLog(tmp_deb);
		}
	#endif

	memcpy(tmp, str+7, 3);
	tmp[3]='\0';
	ret &= strequal(tmp, sensor_name);

	#ifdef DEBUG_LOG_JSON
		if(!ret){
			sprintf(tmp_deb, "\r\n JSON_S check_Name %s", tmp);
			LCD_ErrLog(tmp_deb);
		}
	#endif

	memcpy(tmp, str+13, 3);
	tmp[3]='\0';
	ret &= strequal(tmp, "Val\0");

	#ifdef DEBUG_LOG_JSON
		if(!ret){
			sprintf(tmp_deb, "\r\n JSON_S check_Val %s", tmp);
			LCD_ErrLog(tmp_deb);
		}
	#endif


	memcpy(tmp, str+25, 3);
	tmp[3]='\0';
	ret &= strequal(tmp, "Fmt\0");

	#ifdef DEBUG_LOG_JSON
		if(!ret){
			sprintf(tmp_deb, "\r\n \r\n JSON_S check_Fmt %s", tmp);
			LCD_ErrLog(tmp_deb);
		}
	#endif

	return ret;
}

bool set_room_Valve(char* str, struct room_struct* my_room){
	// {"Id":"02","Eco":"1","sens":[{"Nm":"Tmp","Val":"20.94","Fmt":"C"},{"Nm":"Hum","Val":"62.34","Fmt":"%"}],"acts":[{"Nm":"Vlv","Val":"057","Fmt":"%"}]}
	// {"Nm":"Vlv","Val":"057","Fmt":"%"}
	bool ret = false;
	char value_str[3];
	uint8_t value = 0;
	char format;
	char* str_json_init = str+112;	// {"Nm":"Vlv","Val":"057","Fmt":"%"}

	ret = check_json_actuator(str_json_init, "Vlv\0");
	if(ret){
		memcpy(value_str, str_json_init+19, 3);
		value_str[3]='\0';

		value = str_3d_to_uint8(value_str);
		format = *(str_json_init + 31);

		if ((value >= VALVE_MIN) && (value <= VALVE_MAX) && (format=='%')){
			my_room->valve.value = value;
			my_room->valve.format = format;
			ret = true;
		} else {
			ret = false;
		}
	} else {
		ret = false;
	}

	#ifdef DEBUG_LOG
	if(!ret){
		char tmp[24];
		sprintf(tmp, "\r\n Valve: %03d %c", value, format);
		LCD_ErrLog(tmp);
	}
	#endif
	return ret;
}

bool set_room_Temperature(char* str, struct room_struct* my_room){
	// {"Id":"01","Eco":"1","sens":[{"Nm":"Tmp","Val":"24.00","Fmt":"C"},{"Nm":"Hum","Val":"56.78","Fmt":"%"}]}
	bool ret = false;
	char value_str[6];
	float value;
	char format;
	char* str_json_init = str+29;	// {"Nm":"Tmp","Val":"24.00","Fmt":"C"}

	ret = check_json_sensor(str_json_init, "Tmp\0");
	if(ret){
		memcpy(value_str, str_json_init+19, 5);
		value_str[5]='\0';
		value = str_2_2d_to_float(value_str);
		format = *(str_json_init + 33);
		if (value >= TEMP_MIN && value <= TEMP_MAX && (format=='C' || format=='K')){
			my_room->temperature.value = value;
			my_room->temperature.format = format;
			ret = true;
		} else {
			ret = false;
		}
	} else {
		ret = false;
	}

	#ifdef DEBUG_LOG
	if(!ret){
		char tmp[24];
		sprintf(tmp, "\r\n Temp: %2.2f %c", value, format);
		LCD_ErrLog(tmp);
	}
	#endif
	return ret;
}

bool set_room_Humidity(char* str, struct room_struct* my_room){
	// {"Id":"01","Eco":"1","sens":[{"Nm":"Tmp","Val":"24.00","Fmt":"C"},{"Nm":"Hum","Val":"56.78","Fmt":"%"}]}
	bool ret = false;
	char value_str[6];
	float value;
	char format;
	char* str_json_init = str+66;	// {"Nm":"Hum","Val":"56.78","Fmt":"%"}]}

	ret = check_json_sensor(str_json_init, "Hum\0");
	if(ret){
		memcpy(value_str, str_json_init+19, 5);
		value_str[5]='\0';
		value = str_2_2d_to_float(value_str);
		format = *(str_json_init+33);
		if (value >= HUM_MIN && value <= HUM_MAX && format=='%'){
			my_room->humidity.value = value;
			my_room->humidity.format = format;
			ret = true;
		} else {
			ret = false;
		}
	} else {
		ret = false;
	}

	#ifdef DEBUG_LOG
	if(!ret){
		char tmp[24];
		sprintf(tmp, "\r\n Temp: %2.2f %c", value, format);
		LCD_ErrLog(tmp);
	}
	#endif
	return ret;
}

struct room_struct JSON_to_room_struct(char* str){
	bool check = true;
	struct room_struct my_room;

	check &= set_room_ID(str, &my_room);
	check &= set_room_Eco(str, &my_room);
	check &= set_room_Temperature(str, &my_room);
	check &= set_room_Humidity(str, &my_room);
	check &= set_room_Valve(str, &my_room);

	#ifdef DEBUG_LOG
		char tmp[16];
		sprintf(tmp, "\r\n Check=%d", check);
		LCD_UsrLog(tmp);
		if(!check){
			LCD_ErrLog("\r\n message corrupted!");
//			LCD_ErrLog("\r\n msg: %s", str);
		} else {
			log_room(my_room);
		}
	#endif
	if(check){	// copy old room parameters
		my_room.net_par = rooms[my_room.id-1].net_par;
	}

	return my_room;
}