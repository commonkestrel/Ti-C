#include <keypadc.h>
#include <graphx.h>

#include <stdlib.h>
#include <stdint.h>
#include <time.h>

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

struct {
    Vec position;
    bool down;
    bool right;
} Ball;

void begin(void) {
    gfx_Begin();
    gfx_SetPalette(&palette, 2 * sizeof(uint16_t), 0);
    kb_ClearOnLatch();
}

void end(void) {
    gfx_End();
}

int main(void) {
    clock_t prev = clock();
    kb_Scan();
    bool secondPressed = kb_Data[1] & kb_2nd;

    Vec ball = {0, GFX_LCD_HEIGHT / 2};

    begin();

    while (1) {
        kb_Scan();
        bool second = kb_Data[1] & kb_2nd;

        if (second && !secondPressed) {
            gfx_FillScreen(WHITE);
            gfx_SetColor(BLACK);
            ball.x += 10;
            gfx_FillCircle(ball.x, ball.y, 5);
        }
        secondPressed = second;

        if(kb_On) {
            break;
        }
    }

    end();
    return 0;
}
