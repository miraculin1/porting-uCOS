#include "ekf.h"
#include "Includes.h"
#include "declareFunctions.h"

double ATT_RATE = 100;

// read data from three sensors
static void getMsr(EKF_T *ekf) {
  AccGData(ekf->z, accBias);
  MagmGuassData(ekf->z + 3, magBias);
  GyroRadpSData(ekf->u, gyroBias);
}

// used to gen state vec according to the new msr
void msr2State(EKF_T *ekf) {
  getMsr(ekf);

  // quaternion gen
  double dcm[3][3];
  vecCrossProd(dcm[0], &ekf->z[0], &ekf->z[3]);
  vecCrossProd(dcm[1], dcm[0], &ekf->z[0]);
  vecCrossProd(dcm[2], dcm[0], dcm[1]);
  normalize(dcm[0], 3);
  normalize(dcm[1], 3);
  normalize(dcm[2], 3);
  DCM2quat(ekf->x, dcm);
}

void magBaseCal(EKF_T *ekf) {

  // quaternion init
  double dcm[3][3];
  vecCrossProd(dcm[0], &ekf->z[0], &ekf->z[3]);
  vecCrossProd(dcm[1], dcm[0], &ekf->z[0]);
  vecCrossProd(dcm[2], dcm[0], dcm[1]);
  normalize(dcm[0], 3);
  normalize(dcm[1], 3);
  normalize(dcm[2], 3);
  DCM2quat(ekf->x, dcm);

  // initalize the magBase for later h(x)
  // TODO: figure out the way the init magbase
  double qConj[4];
  double qTmp[4], qTmp1[4];
  double v[4];
  quatConj(ekf->x, qConj);
  vec2Quat(ekf->z + 3, v);

  quatMulQuat(ekf->x, v, qTmp);
  quatMulQuat(qTmp, qConj, qTmp1);
  quat2Vec(qTmp1, ekf->magBase);


  normalize(ekf->magBase, 3);
}

// 1st
// update the quaternion(x) by gyro data
void predictX(EKF_T *ekf) {
  double dq[4];
  quatMulQuat(ekf->x, ekf->u, dq);
  for (int i = 0; i < 4; i++) {
    ekf->x[i] += dq[i] / 2 / ATT_RATE;
  }
}

// 2nd
// need jacobian matrix
void preidctP(EKF_T *ekf) {
  double wx = ekf->z[3], wy = ekf->z[4], wz = ekf->z[5];

  // BUG: bad jacobian
  double F[4 * 4] = {
    1, -wx, -wy, -wz,
    wx, 1, wz, - wy,
    wy, -wz, 1, wx,
    wz, wy, -wx, 1
  };

  double Q[4 * 4] = {0};
  double tmp[4 * 4];
  double tmp1[4 * 4];
  mul(F, ekf->P, false, tmp, 4, 4, 4);
  tran(F, 4, 4);
  mul(tmp, F, false, tmp1, 4, 4, 4);
  add(tmp1, Q, ekf->P, 4, 4, 4);
}

// 3rd
// need jacobian
void calKalGain() {
  /* double H[6 * 4] = { */
    /* {}, */
    /* {}, */
    /* {}, */
    /* {}, */
    /* {}, */
    /* {}, */
    /* {} */
  /* }; */
}

// 4th
// get msr use KalGain to upd predictX, get optimal res
void updX_est(EKF_T *ekf) {}

// 5th
// need jacobian
void updP_est(EKF_T *ekf) {}

static void LPF(double *acc, double *data, double alpha) {
  for (int dim = 0; dim < 3; ++dim) {
    acc[dim] = acc[dim] * (1 - alpha) + alpha * data[dim];
  }
}
