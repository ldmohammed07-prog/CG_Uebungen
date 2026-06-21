#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define PI 3.14159265358979

uint32_t pack(double a, double b, double c) {
    uint32_t qa = (uint32_t)(a / (2*PI) * 2047);
    uint32_t qb = (uint32_t)(b / (2*PI) * 2047);
    uint32_t qc = (uint32_t)(c / (2*PI) * 1023);
    return (qa << 21) | (qb << 10) | qc;
}

void unpack(uint32_t packed, double *a, double *b, double *c) {
    uint32_t qa = (packed >> 21) & 0x7FF;
    uint32_t qb = (packed >> 10) & 0x7FF;
    uint32_t qc =  packed        & 0x3FF;
    *a = qa / 2047.0 * 2*PI;
    *b = qb / 2047.0 * 2*PI;
    *c = qc / 1023.0 * 2*PI;
}

int main() {
    double a = 1.57;
    double b = 0.78;
    double c = 3.14;

    printf("Original:  %f %f %f\n", a, b, c);

    uint32_t packed = pack(a, b, c);
    printf("Gepackt:   %u\n", packed);

    double a2, b2, c2;
    unpack(packed, &a2, &b2, &c2);
    printf("Entpackt:  %f %f %f\n", a2, b2, c2);

    return 0;
}
