#include "util.h"
#include <graphx.h>
#include <ti/real.h>

int floatToStr(float value, char *str, int8_t length) {
    real_t tmp_real = os_FloatToReal(value);
    return os_RealToStr(str, &tmp_real, length, 1, -1);
}

int widthToPixel(float percent) {
    return ((int)(percent * FWIDTH));
}

int heightToPixel(float percent) {
    return ((int)(percent * FHEIGHT));
}
