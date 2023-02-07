/*
 * menu.c
 *
 *  Created on: 22 Ara 2022
 *      Author: aybuke.sokmen
 */


#include "menu.h"
#include "ssd1306.h"
#include "fonts.h"
#include "test.h"


int	count = 0;
#define LCD_CLEAR  SSD1306_Clear()
#define LCD_UPDATE SSD1306_UpdateScreen()
#define START_PIXEL SSD1306_GotoXY(10,crossIndexLine[count])





