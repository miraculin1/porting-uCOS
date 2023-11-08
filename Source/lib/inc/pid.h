#ifndef __PID_H
#define __PID_H

#define PID_DIM 3

typedef struct {
  float lastError[PID_DIM];
  float pram[PID_DIM][3];
  float intergrator[PID_DIM];
  float control[PID_DIM];
} PID_T;

void PID(float *tar, float *cur, float *control);
void initbothPID();
#include "shell.h"
void shellPID(int argc, char *argv[ARGSIZE]);

#endif
