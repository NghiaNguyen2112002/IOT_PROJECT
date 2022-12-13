#include "timer.h"


void timer1_ISR(void){
  SCH_Update();
}

void Init_Timer(void){
  timer1_isr_init();
  timer1_attachInterrupt(timer1_ISR);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);     //DIV16 => 5MHz
  timer1_write(50000);
}
