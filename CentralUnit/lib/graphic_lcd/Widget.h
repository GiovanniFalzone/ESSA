/*!	\file	Button.h
 	\author Daniel Casini
 	\date	10/03/2014
	\brief	Button handling file for the watch
*/

#ifndef WIDGET_H_
#define WIDGET_H_

#include "Event.h"
#include "Touch.h"
#include "fonts.h"


typedef enum  {
	BACKGROUND, BUTTONICON, TEXT, IMAGE
} WidgetType;

typedef enum{
	COLOR, IMG, NONE
} BackgroundMode;

typedef struct {
	unsigned char *image;
	unsigned char *bck_img;
} Image;

typedef struct {
	unsigned char *iconp;
	unsigned char *iconr;
	unsigned char *icon_clear;
	Event		  onpress;
} ButtonIcon;

typedef struct {
	sFONT *font;
/*	unsigned char xsize;
	unsigned char ysize
*/	unsigned short int	color;
	unsigned short int	bcolor;
	char *bimage;
	BackgroundMode bmode;
} Text;

typedef struct {
	unsigned short int xl;
	unsigned short int yt;
	unsigned short int xw;
	unsigned short int yh;
	WidgetType wt;
	void     *ws;
} Widget;

#define txtinfo(w) ((Text *)((w)->ws))
#define biconinfo(w) ((ButtonIcon *)((w)->ws))
#define imginfo(w) ((Image *)((w)->ws))

unsigned char OnTouch(const Widget ws[], TPoint *press);
unsigned char DrawInit(const Widget ws[]);
unsigned char DrawOn(const Widget *w);
unsigned char DrawOff(const Widget *w);
unsigned char WPrint(const Widget *w, char *s);
unsigned char WClear(const Widget *w);

#endif /* BUTTON_H_ */
