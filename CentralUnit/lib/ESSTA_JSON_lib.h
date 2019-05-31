#ifndef ESSTA_JSON_LIB_H
#define ESSTA_JSON_LIB_H

#include "stm32f4xx.h"
#include "ESSTA_definitions.h"

bool strequal(const char* s1, const char* s2);
uint8_t str_to_2d_uint8(char* str);
float str_to_2_2d_float(char *str);
uint8_t check_message(char* str);
struct room_struct JSON_to_room_struct(char* str);

#endif // ESSTA_JSON_LIB_H