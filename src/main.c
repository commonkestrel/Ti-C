#include <keypadc.h>
#include <graphx.h>

#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#include "util.h"

#define COL_BLACK ((unsigned short)0x0000)
#define COL_WHITE ((unsigned short)0xFFFF)
uint16_t palette[2] = 
{
    COL_BLACK,
    COL_WHITE
};
#define BLACK 0
#define WHITE 1

typedef struct Vec {
    uint16_t x;
    uint16_t y;
} Vec;

typedef struct Ball {
    Vec position;
    bool down;
    bool right;
} Ball;

void begin(void);
void end(void);
bool step(float dy, Ball *ball, uint8_t *left, uint8_t *right, uint8_t *primary, uint8_t *secondary);
void draw(uint8_t primary, uint8_t secondary, const Ball *ball, uint8_t left, uint8_t right);

bool prev2nd;
bool prevAlpha;

int main(void) {
    clock_t prev = clock();
    kb_Scan();
    bool secondPressed = kb_Data[1] & kb_2nd;
    bool alphaPressed = kb_Data[6] & kb_Clear;

    Ball ball = {{GFX_LCD_WIDTH / 2, GFX_LCD_HEIGHT / 2}, false, true};
    uint8_t left = GFX_LCD_HEIGHT / 2;
    uint8_t right = GFX_LCD_HEIGHT / 2;

    begin();

    uint8_t primary = WHITE;
    uint8_t secondary = BLACK;

    while (1) {
        draw(primary, secondary, &ball, left, right);
        kb_Scan();
        float dy = ((float)clock()) / ((float)CLOCKS_PER_SEC);
        if(step(dy, &ball, &left, &right, &primary, &secondary)) {
            break;
        }
        gfx_Wait();
    }

    end();
    return 0;
}

void begin(void) {
    gfx_Begin();
    gfx_SetPalette(&palette, 2 * sizeof(uint16_t), 0);
    gfx_SetDrawBuffer();

    kb_ClearOnLatch();
    prev2nd = kb_Data[1] & kb_2nd;
    prevAlpha = kb_Data[2] & kb_Alpha;
}

void end(void) {
    gfx_End();
}

bool step(float dy, Ball *ball, uint8_t *left, uint8_t *right, uint8_t *primary, uint8_t *secondary) {
    bool _second = kb_Data[1] & kb_2nd;

    bool alpha = kb_Data[2] & kb_Alpha;
    *primary = alpha ? BLACK : WHITE;
    *secondary = alpha ? WHITE : BLACK;

    // Move from 1/4 of the screen to 3/4.
    ball->position.x = widthToPixel((sinf(dy) / 4.0f) + 0.5f);
    ball->position.y = widthToPixel((cosf(dy) / 4.0f) + 0.5f) - GFX_LCD_WIDTH / 2 + GFX_LCD_HEIGHT / 2;

    if (kb_Data[7] & kb_Up && *left) {
        *left = *left - 2;
    }
    if (kb_Data[7] & kb_Down && *left < GFX_LCD_HEIGHT) {
        *left = *left + 2;
    }

    return kb_On;
}

void draw(uint8_t primary, uint8_t secondary, const Ball *ball, uint8_t left, uint8_t right) {
    gfx_FillScreen(secondary);
    gfx_SetColor(primary);
    gfx_FillCircle(ball->position.x, ball->position.y, 5);
    gfx_Circle(10, left, 5);
    gfx_Circle(GFX_LCD_WIDTH - 10, right, 5);

    gfx_SwapDraw();
}
