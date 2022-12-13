/*
 * scheduler.h
 *
 *  Created on: Dec 1, 2022
 *      Author: DELL
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_


#include <stdint.h>

typedef struct {
//	pointer to task must be a void (void) function
	void (*pTask) (void);
//	delay (sticks) until the function will (next) be run
	uint32_t Delay;
//	interval (sticks) between subsequent runs
	uint32_t Period;
//	incremented (by scheduler) when task is due to execute
	uint8_t RunMe;

	uint32_t TaskID;
} sTasks;


#define SCH_MAX_TASKS				40
#define	NO_TASK_ID					0


void SCH_Init(void);

uint8_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD);

void SCH_Update(void);

void SCH_Dispatch_Tasks(void);

void SCH_Delete_Task(const uint8_t index);

void SCH_Delete_Task_NAME(	void (*pTask) (void) );

#endif /* INC_SCHEDULER_H_ */
