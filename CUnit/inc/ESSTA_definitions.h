#ifndef ESSTA_TYPES_H
#define ESSTA_TYPES_H

#include <string.h>
#define uint8_t unsigned short int
#define bool uint8_t
#define false 0
#define true 1

//#define DEBUG
//#define DEBUG_LOG
//#define DEBUG_LOG_JSON

#define N_ROOMS 2

#define MSG_LEN	256

#define TEMP_MIN	15
#define TEMP_MAX 	30
#define HUM_MIN		0
#define HUM_MAX		100

#define VALVE_MIN	0
#define VALVE_MAX	100

#define DEFAULT_DES_TEMPERATURE	24.00
#define TEMPERATURE_GOAL_OFFSET	0.5

#define STEP_TEMPERATURE 0.5



struct net_par_struct {
	bool response;
	uint8_t resend;
	bool error;
};

struct sensor_struct {
	float value;
	char format;
};

struct actuator_struct {
	uint8_t value;
	char format;
};

struct room_struct {
	uint8_t id;
	bool eco;
	struct sensor_struct temperature;
	struct sensor_struct humidity;
	struct actuator_struct valve;
	struct net_par_struct net_par;
};

struct home_struct {
	bool eco;
	bool error;
	float des_temperature;
	struct sensor_struct temperature;
	struct sensor_struct humidity;
	struct actuator_struct valve;
};

extern struct room_struct rooms[N_ROOMS];
extern struct home_struct home_struct;
#endif //ESSTA_TYPES_H
