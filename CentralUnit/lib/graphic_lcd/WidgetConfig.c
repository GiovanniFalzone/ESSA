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
	icontime_on, icontime_off, E_LEFT_ARROW
};

ButtonIcon button_right_arrow = {
	icontimeset_on, icontimeset_off, E_RIGHT_ARROW
};

ButtonIcon button_settings = {
	iconalarm_on, iconalarm_off, E_SETTINGS
};

ButtonIcon button_eco = {
	button_plus_on, button_plus_off, E_ECO
};

ButtonIcon button_plus = {
	button_postpone_on, button_postpone_off, E_PLUS
};

ButtonIcon button_minus = {
	button_postpone_on, button_postpone_off, E_MINUS
};

//---------------------
Image image_alert = {
	iconswatch_on
};

Image image_temperature = {
	button_minus_on
};
Image image_humidity = {
	button_alarm_on
};

//---------------------
const Text Digits[2]={
	{ &Font16x24, White, Black, Bck_hours, COLOR },	// temperature
	{ &Font16x24, White, Black, Bck_minutes, COLOR },	// humidity
};

Image backg = {
	ImageBuffer
};

const Widget Screen_objects[NUMWIDGETS] = {
	{0, 0, 320, 240, BACKGROUND, (void *)&backg},
	{0, 175, 64, 64, BUTTONICON, (void *)&button_left_arrow},
	{89, 175, 64, 64, BUTTONICON, (void *)&button_right_arrow},
	{174, 175, 64, 64, BUTTONICON, (void *)&button_settings},
	{278, 36-32, 32, 32, BUTTONICON, (void *)&button_eco},
	{278, 146-32, 32, 32, BUTTONICON, (void *)&button_plus},
	{278, 146-32, 32, 32, BUTTONICON, (void *)&button_minus},

	{253, 175, 64, 64, IMAGE, (void *)&image_alert},
	{278, 72-32, 32, 32, IMAGE, (void *)&image_temperature},
	{278, 109-32, 32, 32, IMAGE, (void *)&image_humidity},

	{100, 64, 	80, 40, TEXT, (void *)&Digits[0]},		// Temperature
	{100, 64+40, 80, 40, TEXT, (void *)&Digits[1]},		// Humidity
};

