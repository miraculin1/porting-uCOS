#include "Includes.h"

OS_STK __stk_Array[STK_CNT][STK_SIZE];
OS_MEM *stkpool;
OS_STK mainStk[STK_SIZE * 3];
OS_STK *topMainSTK = &mainStk[STK_SIZE - 1];

/*******************************
 * Table of tasks
 *******************************
 * prio|name|note
 * */
void userTaskCreate() {
  OS_STK *pstk;
  INT8U ERROR;

  pstk = OSMemGet(stkpool, &ERROR);
  OSTaskCreate(&updateThro, NULL, &pstk[STK_SIZE - 1], 1);

  pstk = OSMemGet(stkpool, &ERROR);
  OSTaskCreate(&SendInfo, NULL, &pstk[STK_SIZE - 1], 4);
}

int main() {

  INT8U ERROR;

  while (1) {
    OSInit();

    stkpool =
        OSMemCreate(&__stk_Array, STK_CNT, sizeof(OS_STK) * STK_SIZE, &ERROR);

    OS_STK *pstk = OSMemGet(stkpool, &ERROR);
    // MUST
    OSTaskCreate(&initSys, NULL, &pstk[STK_SIZE - 1], 0);

    OSStart();
  }
}

// this func MUST run as first task
// after initalize, it will delet itself
// - Set up systick
// - Set pendSV priority
void initSys() {
  initSystickPsv();

  OSStatInit();
  initHardware();
  userTaskCreate();
  OSTaskDel(OS_PRIO_SELF);
  while (1) {
  }
}

void initHardware() {
  initLED();
  initUSART();
  initIIC();
  initMPU6050();
  initHMC();
  initRec();
  initMotor();
}

void accConvert(int16_t data, uint16_t scale, char *buffer) {
  int res = data * 100 * scale / (0xffff / 2);
  sprintf(buffer, "%d", res);
}

void SendInfo() {
  int16_t data[3];
  char buffer[100];
  while (1) {
    OSSchedLock();
    AccData(data);
    OSSchedUnlock();
    accConvert(data[0], 2, buffer);
    USendStr(buffer);
    USendByte(';');
    accConvert(data[1], 2, buffer);
    USendStr(buffer);
    USendByte(';');
    accConvert(data[2], 2, buffer);
    USendStr(buffer);
    USendByte(';');
    USendByte('\n');

    OSTimeDly(100);
  }
}

void updateThro() {
  while (1) {
    if (recData.linedUp) {
      fourMotro[0] = recData.chs[2] - 1000;
      setThro(fourMotro);
    }
    OSTimeDly(2);
  }
}

void initSystickPsv() {
  // set overall priority group to
  // 4bit preemt, 4bit sub
  SCB->AIRCR |= (0b101 << 8);
  // set up systick
  // /8 scale
  SysTick->CTRL &= ~(1 << 2);
  SysTick->LOAD = ((SystemCoreClock / 8 / OS_TICKS_PER_SEC) & 0xFFFFFF);
  // enable systick inttrupt
  SysTick->CTRL |= (1 << 1);
  SysTick->CTRL |= 1;

  // pendSV set to lowest priority
  SCB->SHP[9] = 0xFF;
}
