#include <keypadc.h>
#include <graphx.h>
#include <ti/error.h>
#include <ti/getcsc.h>
#include <ti/flags.h>
#include <sys/util.h>
#include <sys/rtc.h>

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

#define PADDLE_HEIGHT 60
#define PADDLE_WIDTH 10
// Add 6 pixels of padding to each side
#define PADDLE_MIN 6
#define PADDLE_MAX GFX_LCD_HEIGHT - PADDLE_HEIGHT - 6

#define BALL_RAD 5

typedef struct Vec {
    uint16_t x;
    uint16_t y;
} Vec;

typedef struct Ball {
    Vec position;
    bool down;
    bool right;
} Ball;

// Forward function definitions
void begin(void);
void end(void);
bool gameOver(uint8_t side, uint8_t primary, uint8_t *left_points, uint8_t *right_points);
bool step(Ball *ball, uint8_t *left, uint8_t *right, uint8_t *primary, uint8_t *secondary, uint8_t *left_points, uint8_t *right_points, bool *collided);
void draw(uint8_t primary, uint8_t secondary, const Ball *ball, uint8_t left, uint8_t right);
bool colliding(Ball *ball, uint8_t left, uint8_t right);

bool prev2nd;
bool prevAlpha;

int main(void) {
    Ball ball = {{GFX_LCD_WIDTH / 2, GFX_LCD_HEIGHT / 2}, randInt(0, 1), randInt(0, 1)};
    uint8_t left = (GFX_LCD_HEIGHT-PADDLE_HEIGHT) / 2;
    uint8_t right = (GFX_LCD_HEIGHT-PADDLE_HEIGHT) / 2;

    begin();

    uint8_t primary = WHITE;
    uint8_t secondary = BLACK;

    uint8_t left_points = 0;
    uint8_t right_points = 0;

    bool collided = false;

    while (1) {
        draw(primary, secondary, &ball, left, right);
        kb_Scan();

        uint8_t stepped = step(&ball, &left, &right, &primary, &secondary, &left_points, &right_points, &collided);
        if(stepped)
            break;

        if (kb_On)
            break;

        gfx_Wait();
    }

    end();
    return 0;
}

void begin(void) {
    srandom(rtc_Time());

    gfx_Begin();
    gfx_SetPalette(&palette, 2 * sizeof(uint16_t), 0);
    gfx_SetDrawBuffer();
    gfx_SetTextScale(3, 3);

    kb_ClearOnLatch();
    prev2nd = kb_Data[1] & kb_2nd;
    prevAlpha = kb_Data[2] & kb_Alpha;
}

void end(void) {
    gfx_End();
}

bool gameOver(const uint8_t side, uint8_t primary, uint8_t *left_points, uint8_t *right_points) {
    *left_points = 0;
    *right_points = 0;

    gfx_SetTextFGColor(primary);

    const char *over = "Game Over!";
    gfx_PrintStringXY(over, (GFX_LCD_WIDTH-gfx_GetStringWidth(over))/2, GFX_LCD_HEIGHT/2-(8*3));
    
    if (side == 1) {
        const char *right = "Right wins!";
        gfx_PrintStringXY(right, (GFX_LCD_WIDTH-gfx_GetStringWidth(right))/2, GFX_LCD_HEIGHT/2);
    } else {
        const char *left = "Left wins!";
        gfx_PrintStringXY(left, (GFX_LCD_WIDTH-gfx_GetStringWidth(left))/2, GFX_LCD_HEIGHT/2);
    }

    gfx_SwapDraw();
    kb_Scan();
    while (!(kb_Data[6] & kb_Enter)) {
        kb_Scan();
        if (kb_On) {
            return true;
        }
    }

    return false;
}

void reset(Ball *ball, uint8_t *left, uint8_t *right) {
    ball->position.x = GFX_LCD_WIDTH/2;
    ball->position.y = GFX_LCD_HEIGHT/2;
    ball->down = randInt(0, 1);
    ball->right = randInt(0, 1);
    *left = (GFX_LCD_HEIGHT-PADDLE_HEIGHT) / 2;
    *right = (GFX_LCD_HEIGHT-PADDLE_HEIGHT) / 2;
}

bool step(Ball *ball, uint8_t *left, uint8_t *right, uint8_t *primary, uint8_t *secondary, uint8_t *left_points, uint8_t *right_points, bool *collided) {
    if (ball->position.x <= 10) {
        reset(ball, left, right);
        *right_points += 1;
    } else if (ball->position.x >= GFX_LCD_WIDTH - 10) {
        reset(ball, left, right);
        *left_points += 1;
    }
    
    bool invert = (*right_points + *left_points) % 2;
    *primary = invert ? BLACK : WHITE;
    *secondary = invert ? WHITE : BLACK;

    if (*right_points >= 7) {
        return gameOver(1, *secondary, left_points, right_points);
    } else if (*left_points >= 7) {
        return gameOver(2, *secondary, left_points, right_points);
    }

    if (colliding(ball, *left, *right)) {
        if (!*collided) {
            ball->right = !ball->right;
            *collided = true;
        }
    } else {
        *collided = false;
    }

    ball->position.x = ball->right ? ball->position.x + 2 : ball->position.x - 2;

    if (ball->position.y <= BALL_RAD || ball->position.y >= GFX_LCD_HEIGHT - BALL_RAD) {
        ball->down = !ball->down;
    }
    ball->position.y = ball->down ? ball->position.y + 3 : ball->position.y - 3;

    if (kb_Data[7] & kb_Up && *right > PADDLE_MIN) {
        *right = *right - 2;
    }
    if (kb_Data[7] & kb_Down && *right < PADDLE_MAX) {
        *right = *right + 2;
    }

    if (kb_Data[1] & kb_2nd && *left > PADDLE_MIN) {
        *left = *left - 2;
    }
    if (kb_Data[2] & kb_Alpha && *left < PADDLE_MAX) {
        *left = *left + 2;
    }

    return false;
}

bool colliding(Ball *ball, uint8_t left, uint8_t right) {
    bool left_collision =  (ball->position.x <= PADDLE_WIDTH + 10 + BALL_RAD && ball->position.y - BALL_RAD <= left + PADDLE_HEIGHT && ball->position.y + BALL_RAD >= left);
    bool right_collision =  (ball->position.x >= GFX_LCD_WIDTH - 10 - PADDLE_WIDTH - BALL_RAD && ball->position.y - BALL_RAD <= right + PADDLE_HEIGHT && ball->position.y + BALL_RAD >= right);
    return left_collision || right_collision;
}

void draw(uint8_t primary, uint8_t secondary, const Ball *ball, uint8_t left, uint8_t right) {
    gfx_FillScreen(secondary);
    gfx_SetColor(primary);
    gfx_FillRectangle(ball->position.x - BALL_RAD, ball->position.y - BALL_RAD, BALL_RAD*2, BALL_RAD*2);
    gfx_FillRectangle(10, left, PADDLE_WIDTH, PADDLE_HEIGHT);
    gfx_FillRectangle(GFX_LCD_WIDTH - 10 - PADDLE_WIDTH, right, PADDLE_WIDTH, PADDLE_HEIGHT);

    gfx_SwapDraw();
}
