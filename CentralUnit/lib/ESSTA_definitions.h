#ifndef ESSTA_TYPES_H
#define ESSTA_TYPES_H

#define bool uint8_t
#define false 0
#define true 1

#define DEBUG
//#define DEBUG_LOG
//#define DEBUG_LOG_JSON

#define MAX_ROOMS	2

#define MSG_LEN	256

#define TEMP_MIN	0
#define TEMP_MAX 	50
#define HUM_MIN		0
#define HUM_MAX		100

#define N_ROOMS 2

struct net_par_struct {
	bool response;
	uint8_t resend;
	bool error;
};

struct sensor_struct {
	float value;
	char format;
};

struct room_struct {
	uint8_t id;
	bool eco;
	struct sensor_struct temperature;
	struct sensor_struct humidity;
	struct net_par_struct net_par;
};
extern struct room_struct rooms[MAX_ROOMS];
#endif //ESSTA_TYPES_H