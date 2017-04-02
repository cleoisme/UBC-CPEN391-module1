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
#define BT_MAP_INIT 'W'
#define BT_MAP_DELIMITER 'Q'
#define BT_MAP_FIELD_DELIMITER 'V'
#define MAX_MAPS 20
#define MAX_DATA_SIZE 50

typedef enum{
	None,
	Weather,
	Rating,
	Map,
} State;


#endif /* HIKINGPAL_H_ */
