#ifndef ESSTA_LIB_H
#define ESSTA_LIB_H

#define DEBUG

#define bool uint8_t
#define false 0
#define true 1

#define MAX_ROOMS	2

#define MSG_LEN	256

#define TEMP_MIN	0
#define TEMP_MAX 	50
#define HUM_MIN		0
#define HUM_MAX		100

//----------------------

struct sensor_struct {
	float value;
	char format;
};

struct room_struct {
	uint8_t id;
	bool eco;
	struct sensor_struct temperature;
	struct sensor_struct humidity;
};

struct room_struct rooms[MAX_ROOMS];


//----------------------
void init_rooms(){
	uint8_t i = 0;
	for(i=0;i++;i<MAX_ROOMS){
		rooms[i].id=-1;
		rooms[i].eco=0;
		rooms[i].temperature.value=0.0;
		rooms[i].temperature.format='?';
		rooms[i].humidity.value=0.0;
		rooms[i].humidity.format='?';
	}
}
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

uint8_t str_to_2d_uint8(char* str){
	uint8_t ret = (str[0] - '0')*10;
	ret += (str[1] - '0');
	return ret;
}

float str_to_2_2d_float(char *str){	//01.23
	float ret = (float)str_to_2d_uint8(str);
	ret += ((float)str_to_2d_uint8(str+3))/100;
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

	#ifdef DEBUG
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
		id = str_to_2d_uint8(value);
		if (id > 0 && id < 8){
			my_room->id = id;
			ret = true;
		} else {
			ret = false;
		}
	} else {
		ret = false;
	}

	#ifdef DEBUG
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

	#ifdef DEBUG
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

	#ifdef DEBUG
		char tmp_deb[32];
		if(!ret){
			sprintf(tmp_deb, "\r\n JSON_S check %s", tmp);
			LCD_ErrLog(tmp_deb);
		}
	#endif

	memcpy(tmp, str+7, 3);
	tmp[3]='\0';
	ret &= strequal(tmp, sensor_name);

	#ifdef DEBUG
		if(!ret){
			sprintf(tmp_deb, "\r\n JSON_S check %s", tmp);
			LCD_ErrLog(tmp_deb);
		}
	#endif

	memcpy(tmp, str+13, 3);
	tmp[3]='\0';
	ret &= strequal(tmp, "Val\0");

	#ifdef DEBUG
		if(!ret){
			sprintf(tmp_deb, "\r\n JSON_S check %s", tmp);
			LCD_ErrLog(tmp_deb);
		}
	#endif


	memcpy(tmp, str+27, 3);
	tmp[3]='\0';
	ret &= strequal(tmp, "Fmt\0");

	#ifdef DEBUG
		if(!ret){
			sprintf(tmp_deb, "\r\n %s", tmp);
			LCD_ErrLog(tmp_deb);
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
		value = str_to_2_2d_float(value_str);
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

	#ifdef DEBUG
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
		value = str_to_2_2d_float(value_str);
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

	#ifdef DEBUG
	if(!ret){
		char tmp[24];
		sprintf(tmp, "\r\n Temp: %2.2f %c", value, format);
		LCD_ErrLog(tmp);
	}
	#endif
	return ret;
}

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

bool JSON_to_room_struct(char* str){
	bool check = true;
	struct room_struct my_room;

	check &= set_room_ID(str, &my_room);
	check &= set_room_Eco(str, &my_room);
	check &= set_room_Temperature(str, &my_room);
	check &= set_room_Humidity(str, &my_room);

	if(check){
		rooms[my_room.id-1] = my_room;
	}

	#ifdef DEBUG
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
	return check;
}

#endif // ESSTA_LIB_H