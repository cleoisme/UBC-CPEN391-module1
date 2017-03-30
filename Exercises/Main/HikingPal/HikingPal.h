/*
 * HikingPal.h
 *
 *  Created on: Mar 30, 2017
 *      Author: Anthony
 */

#ifndef HIKINGPAL_H_
#define HIKINGPAL_H_

#define BT_RATE_TRAIL 'Q'
#define BT_WEATHER 'Z'
#define BT_MAP 'X'
#define MAX_MAPS 20

typedef enum{
	None,
	Weather,
	Rating,
	Map,
} State;


#endif /* HIKINGPAL_H_ */
