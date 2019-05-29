#ifndef ESSTA_GRAPHIC_H
#define ESSTA_GRAPHIC_H

#include "stm32f4xx.h"
#include "stm32f4_discovery_lcd.h"
#include <math.h>

#include "ESSTA_definitions.h"

#include "graphic_lcd/STMPE811QTR.h"
#include "graphic_lcd/pictures.h"
#include "graphic_lcd/Widget.h"
#include "graphic_lcd/WidgetConfig.h"
#include "graphic_lcd/Touch.h"
#include "graphic_lcd/Event.h"
#include "graphic_lcd/Stopwatch_SM.h"
#include "graphic_lcd/lcd_add.h"
#include "graphic_lcd/fonts.h"
#include "graphic_lcd/debug.h"

#define HOME_PAGE			0
#define ROOM1_PAGE			1
#define ROOM2_PAGE			2
#define ROOM1_SETTINGS_PAGE	3
#define ROOM2_SETTINGS_PAGE	4

struct display_data {
	uint8_t id;
	float temperature;
	float humidity;
	bool eco;
	bool error;
};

uint8_t page;
struct display_data display_data;

void graphic_init();
void graphic_step();

void graphic_update();

#endif /* ESSTA_GRAPHIC_H */
