#ifndef __CALI_H
#define __CALI_H

#include <stdint.h>

extern uint8_t CALIBDONE;
extern double accBias[6];
extern double magBias[6];
extern double gyroBias[3];

#define SAMPLE_NUM 10
void caliAcc();
void caliGyro();
void caliMag();

#endif
