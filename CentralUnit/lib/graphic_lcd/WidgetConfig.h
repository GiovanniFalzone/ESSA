/*
 * WidgetConfig.h
 *
 *  Created on: 22/ott/2015
 *      Author: admim
 */

#ifndef WIDGETCONFIG_H_
#define WIDGETCONFIG_H_

#define NUMWIDGETS 12

#define BAKCG 			0
#define B_LEFT_ARROW 	1
#define B_RIGHT_ARROE 	2
#define B_SETTINGS 		3
#define B_ECO 			4
#define B_PLUS 			5
#define B_MINUS 		6

#define I_ALERT 		7
#define I_TEMPERATURE 	8
#define I_HUMIDITY 		9

#define T_TEMPERATURE 		10
#define T_HUMIDITY 			11

#define E_LEFT_ARROW 	0x01
#define E_RIGHT_ARROW 	0x02
#define E_SETTINGS 		0x04
#define E_ECO 			0x10
#define E_PLUS			0x20
#define E_MINUS			0x40

extern const Widget Screen_objects[NUMWIDGETS];

#endif /* WIDGETCONFIG_H_ */
