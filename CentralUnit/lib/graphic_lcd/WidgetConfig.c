/*
 * WidgetConfig.c
 *
 *  Created on: 21/ott/2015
 *      Author: admim
 */

#include "Widget.h"
#include "WidgetConfig.h"
#include "pictures.h"
#include <stdio.h>
#include "stm32f4_discovery_lcd.h"
#include "fonts.h"

ButtonIcon button_left_arrow = {
	img_left_arrow, img_left_arrow, NULL, E_LEFT_ARROW
};

ButtonIcon button_right_arrow = {
	img_right_arrow, img_right_arrow, NULL, E_RIGHT_ARROW
};

ButtonIcon button_settings = {
	img_Settings, img_Settings, NULL, E_SETTINGS
};

ButtonIcon button_eco = {
	img_Eco, img_bck_Eco, NULL, E_ECO
};

ButtonIcon button_plus = {
	img_Plus_on, img_Plus_off, img_bck_Plus, E_PLUS
};

ButtonIcon button_minus = {
	img_Minus_off, img_Minus_on, img_bck_Minus, E_MINUS
};

//---------------------
Image image_home = {
	img_Home, NULL
};
Image image_room = {
	img_Room, NULL
};

Image image_warning = {
	img_Warning, img_bck_Warning
};

Image image_hot_temperature = {
	img_Hot, NULL
};

Image image_cold_temperature = {
	img_Cold, NULL
};

Image image_humidity = {
	img_Humidity, NULL
};

Image image_valve = {
	img_Valve, NULL
};

Image backg = {
	img_background, NULL
};

//---------------------
const Text Digits[4]={
	{ &Font16x24, Black, Black, img_bck_temp_text, IMG },	// temperature
	{ &Font16x24, Black, Black, img_bck_hum_text, IMG },	// humidity
	{ &Font16x24, Black, Black, img_bck_valve_text, IMG },	// valve
	{ &Font16x24, Black, Black, img_bck_room_id, IMG },		// room id
};

const Widget Screen_objects[NUMWIDGETS] = {
	{0,			0, 			320, 240, 	BACKGROUND, (void *)&backg},

	{8,			66, 		18, 128, 	BUTTONICON, (void *)&button_left_arrow},
	{298, 		66, 		18, 128, 	BUTTONICON, (void *)&button_right_arrow},
	{267, 		4, 			32, 32, 	BUTTONICON, (void *)&button_settings},
	{25,	 	0,		 	32, 32, 	BUTTONICON, (void *)&button_eco},
	{199, 		64, 		32, 32, 	BUTTONICON, (void *)&button_plus},
	{245, 		64, 		32, 32, 	BUTTONICON, (void *)&button_minus},

	{83, 		0, 			32, 32, IMAGE, (void *)&image_warning},
	{54,	 	64, 		32, 32, IMAGE, (void *)&image_hot_temperature},
	{54,	 	64, 		32, 32, IMAGE, (void *)&image_cold_temperature},
	{54, 		130, 		32, 32, IMAGE, (void *)&image_humidity},
	{54, 		188, 		32, 32, IMAGE, (void *)&image_valve},
	{149, 		4, 			32, 32, IMAGE, (void *)&image_home},
	{149, 		4, 			32, 32, IMAGE, (void *)&image_room},

	{100, 		64, 		100, 40, TEXT, (void *)&Digits[0]},		// Temperature
	{100, 		130, 		100, 40, TEXT, (void *)&Digits[1]},		// Humidity
	{100,	 	190, 		100, 40, TEXT, (void *)&Digits[2]},		// Valve
	{149+32+4, 	8, 			32, 32, TEXT, (void *)&Digits[3]},		// Room Id
};

