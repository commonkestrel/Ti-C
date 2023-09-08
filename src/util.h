#include <stdint.h>

#define FWIDTH ((float)GFX_LCD_WIDTH)
#define FHEIGHT ((float)GFX_LCD_HEIGHT)

int floatToStr(float value, char *str, int8_t length);

int widthToPixel(float percent);
int heightToPixel(float percent);