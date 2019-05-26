#ifndef ESSTA_JSON_LIB_H
#define ESSTA_JSON_LIB_H

#include "stm32f4xx.h"
#include "lcd_log.h"

extern bool strequal(const char* s1, const char* s2);
extern uint8_t str_to_2d_uint8(char* str);
extern float str_to_2_2d_float(char *str);
extern uint8_t check_message(char* str);
extern bool set_room_ID(char* str, struct room_struct* my_room);
extern bool set_room_Eco(char* str, struct room_struct* my_room);
extern bool check_json_sensor(char* str, char* sensor_name);
extern bool set_room_Temperature(char* str, struct room_struct* my_room);
extern bool set_room_Humidity(char* str, struct room_struct* my_room);
extern uint8_t JSON_to_room_struct(char* str);

#endif // ESSTA_JSON_LIB_H