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
#include "graphic_lcd/lcd_add.h"
#include "graphic_lcd/fonts.h"
#include "graphic_lcd/debug.h"

struct display_data {
	uint8_t id;
	bool eco;
	bool error;
	struct sensor_struct temperature;
	struct sensor_struct humidity;
	struct actuator_struct valve;
};

typedef enum Signal {
	ENTRY_SIG, EXIT_SIG, LEFT_ARROW_SIG, RIGHT_ARROW_SIG, SETTINGS_SIG, ECO_SIG, PLUS_SIG, MINUS_SIG, NULL_SIG
} Signal_t;

enum State_Control {
	HOME, HOME_SETTINGS, ROOM, ROOM_SETTINGS
};


enum State_Control State_Control;
struct display_data display_data, old_display_data;
uint8_t room_selector;

void graphic_init();
void graphic_step();

void touch_event_step();

#endif /* ESSTA_GRAPHIC_H */
