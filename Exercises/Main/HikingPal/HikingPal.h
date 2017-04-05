/*
 * HikingPal.h
 *
 *  Created on: Mar 30, 2017
 *      Author: Anthony
 */

#ifndef HIKINGPAL_H_
#define HIKINGPAL_H_

#define BT_RATE_TRAIL 'P'
#define BT_WEATHER 'Z'
#define BT_GPS 'Y'
#define BT_MAP_INIT 'X'
#define BT_MAP_DELIMITER 'Q'
#define BT_MAP_FIELD_DELIMITER 'U'
#define MAX_MAPS 20
#define MAX_DATA_SIZE 50

typedef enum{
	None,
	Weather,
	Gps,
	Rating,
	Map,
} State;


#endif /* HIKINGPAL_H_ */
