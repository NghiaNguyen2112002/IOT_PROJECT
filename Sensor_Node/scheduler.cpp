/*
 * scheduler.cpp
 *
 *  Created on: Nov 17, 2022
 *      Author: DELL
 */


#include "scheduler.h"

static sTasks SCH_tasks_G[SCH_MAX_TASKS];

void SCH_Init(void){
	uint8_t i;
	for(i = 0; i < SCH_MAX_TASKS; i++){

	}
}

uint8_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD){
	uint8_t index = 0;
	while( (SCH_tasks_G[index].pTask != 0) && (index < SCH_MAX_TASKS) ){
		index++;
	}

//	if index == MAX_TASKS => list is full
	if(index == SCH_MAX_TASKS){
		return SCH_MAX_TASKS;
	}

//	there is a space in the task array
	SCH_tasks_G[index].pTask 	= pFunction;
	SCH_tasks_G[index].Delay 	= DELAY;
	SCH_tasks_G[index].Period 	= PERIOD;
	SCH_tasks_G[index].RunMe 	= 0;
	return index;
}

void SCH_Update(void){
	uint8_t index;
//	NOTE: calculations are in STICKS (NOT milliseconds)
	for(index = 0; index < SCH_MAX_TASKS; index++){
//		check if there is a task at this location
		if(SCH_tasks_G[index].pTask){
			if(SCH_tasks_G[index].Delay == 0){
//				increase RunMe flag
				SCH_tasks_G[index].RunMe++;

//				if(SCH_tasks_G[index]. Period){
					SCH_tasks_G[index].Delay = SCH_tasks_G[index].Period;
//				}
			}
			else{
//				delay > 0
				SCH_tasks_G[index].Delay--;
			}
		}
	}
}

void SCH_Dispatch_Tasks(void){
	uint8_t index;
	for(index = 0; index < SCH_MAX_TASKS; index++){
		if(SCH_tasks_G[index].RunMe > 0){
//			run the task
			(*SCH_tasks_G[index].pTask)();
			SCH_tasks_G[index].RunMe--;

			//if period == 0 => one shot task => delete after run
			if(SCH_tasks_G[index].Period == 0){
				SCH_Delete_Task(index);
			}
		}
	}
}


void SCH_Delete_Task(const uint8_t index){
	if(SCH_tasks_G[index].pTask == 0){
//		no task at this location
		return;
	}
	SCH_tasks_G[index].pTask 	= 0x0000;
	SCH_tasks_G[index].Delay 	= 0;
	SCH_tasks_G[index].Period 	= 0;
	SCH_tasks_G[index].RunMe 	= 0;
	SCH_tasks_G[index].TaskID 	= 0x0000;
}

void SCH_Delete_Task_NAME(	void (*pTask) (void) ){
	for(uint8_t i = 0; i < SCH_MAX_TASKS; i++){
		if(SCH_tasks_G[i].pTask == pTask){
			SCH_Delete_Task(i);
			return;
		}
	}
}

