/*
 * temperature_sensor.h
 *
 *  Created on: Oct 15, 2016
 *      Author: Harvard Tseng
 */

#ifndef TEMPERATURE_SENSOR_H_
#define TEMPERATURE_SENSOR_H_

extern void tempsensor_init(void);
extern void tempsensor_driver(void);

extern struct rtos_pipe *tempsensor_Fifo;

#endif /* TEMPERATURE_SENSOR_H_ */
