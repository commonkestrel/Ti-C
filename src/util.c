#include <ti/real.h>

int floatToStr(float value, char *str, int8_t length) {
    real_t tmp_real = os_FloatToReal(value);
    return os_RealToStr(str, &tmp_real, length, 1, -1);
}