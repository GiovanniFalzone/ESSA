#include "DHT.h"
#include <Servo.h>

#define DEBUG

#define TASK_PERIOD 2000
#define CONTROL_VALVE_PERIOD 4000
#define SEND_PERIOD	5000

#define ERROR_LED_PIN 13
#define ENERGY_SAVING_LED_PIN 12
#define GOAL_ACHIEVED_LED_PIN 11

#define DHT_PIN	10
#define VALVE_PIN	9

#define DEFAULT_ID							01
#define DEFAULT_MAX_TEMP					28
#define DEFUAILT_MIN_TEMP					16
#define DEFAULT_ENERGY_SAVING_DIFFERENCE	2
#define DEFAULT_DESIRED_TEMPERATURE			24

#define VALVE_CLOSED	20
#define VALVE_LOW		45
#define VALVE_MIDDLE	90
#define VALVE_HIGH		135
#define VALVE_OPEN		160

#define HIGH_VALUE_DIFFERENCE 2
#define APPROACH_TEMP_DIFFERENCE 1

//------------------------------

struct room_settings {
	unsigned int Id;
	unsigned int max_temp;
	unsigned int min_temp;
	unsigned int energy_saving_difference;
	unsigned int desired_temperature;
};

struct room_status{
	float temp;
	float hum;
	bool movement;
	unsigned int valve_status;
};

struct room {
	struct room_settings settings;
	struct room_status status;
};

struct room my_room = {{DEFAULT_ID, DEFAULT_MAX_TEMP, DEFUAILT_MIN_TEMP, DEFAULT_ENERGY_SAVING_DIFFERENCE, DEFAULT_DESIRED_TEMPERATURE},{0,0,0,0}};

DHT dht(DHT_PIN,DHT22);
Servo valve;

void move_valve(unsigned int pos){
	valve.write(pos);
	delay(15);
	my_room.status.valve_status = pos;
}

void init_valve() {
	unsigned int pos = 0;
	valve.attach(VALVE_PIN);
	for(pos=VALVE_CLOSED; pos<=VALVE_OPEN; pos++){
		move_valve(pos);
	}
	for(pos=VALVE_OPEN; pos>=VALVE_CLOSED; pos--){
		move_valve(pos);
	}
}

void init_leds(){
	pinMode(ERROR_LED_PIN, OUTPUT);
	pinMode(ENERGY_SAVING_LED_PIN, OUTPUT);
	pinMode(GOAL_ACHIEVED_LED_PIN, OUTPUT);
	// check
	digitalWrite(ERROR_LED_PIN, HIGH);
	delay(200);
	digitalWrite(ENERGY_SAVING_LED_PIN, HIGH);
	delay(200);
	digitalWrite(GOAL_ACHIEVED_LED_PIN, HIGH);
	delay(200);
	digitalWrite(ERROR_LED_PIN, LOW);
	delay(200);
	digitalWrite(ENERGY_SAVING_LED_PIN, LOW);
	delay(200);
	digitalWrite(GOAL_ACHIEVED_LED_PIN, LOW);
}

void init_DHT(){
	dht.begin();
	bool ls = false;
	while(!dht_sensor_task_run()){
		if(ls) {
			digitalWrite(ERROR_LED_PIN, HIGH);
		}
		else {
			digitalWrite(ERROR_LED_PIN, LOW);
		}
		delay(100);
		ls = !ls;
	}
	 digitalWrite(ERROR_LED_PIN, LOW);
}

void init_communication(){
	Serial.begin(9600);
}

void setup() {
	init_communication();
	init_leds();
	init_valve();
	init_DHT();
}

void loop() {
	static uint32_t  this_run=0, last_run=0, control_valve_last=0, send_last=0;
	this_run = millis();	
	if((this_run-last_run) >= TASK_PERIOD){
		last_run = this_run;
		dht_sensor_task_run();
		if((this_run - control_valve_last) >= CONTROL_VALVE_PERIOD){
			control_valve_last = this_run;
			control_valve();
		}
		if((this_run - send_last) >= SEND_PERIOD){
			send_last = this_run;
			send_room_status();
		}
	}
}

void control_valve(){
	float temp_err_diff = my_room.status.temp - my_room.settings.desired_temperature;
	// low temperature
	if(temp_err_diff < -HIGH_VALUE_DIFFERENCE){
		move_valve(VALVE_OPEN);
	} else if(temp_err_diff < -APPROACH_TEMP_DIFFERENCE){
		move_valve(VALVE_HIGH);
	}

	// high temperature
	if(temp_err_diff > HIGH_VALUE_DIFFERENCE){
		move_valve(VALVE_CLOSED);
	} else if (temp_err_diff > APPROACH_TEMP_DIFFERENCE){
		move_valve(VALVE_LOW);
	}

	if((temp_err_diff >= -APPROACH_TEMP_DIFFERENCE) &&  (temp_err_diff <= APPROACH_TEMP_DIFFERENCE)){
		move_valve(VALVE_MIDDLE);
		digitalWrite(GOAL_ACHIEVED_LED_PIN, HIGH);
	}
}

bool dht_sensor_task_run() {
	float hum = dht.readHumidity();
	float temp = dht.readTemperature();
	if (isnan(temp) || isnan(hum)) {
		#ifdef DEBUG
			Serial.println("Impossibile leggere il sensore!");
		#endif
		return false;
	} else {
		my_room.status.hum = hum;
		my_room.status.temp = temp;
		return true;
	}
}

void send_room_status(){
	char str_temp[6], str_hum[6], str[256];
	dtostrf(my_room.status.temp, 4, 2, str_temp); /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
	dtostrf(my_room.status.hum, 4, 2, str_hum);
	sprintf(str, "{\"Id\":\"%02d\",\"Sensors\":[{\"name\":\"temperature\",\"format\":\"C\",\"value\":\"%s\"},{\"name\":\"humidity\",\"format\":\"%%\",\"value\":\"%s\"},{\"name\":\"pir\",\"format\":\"bool\",\"value\":\"%1d\"}], \"Actuators\":[{\"name\":\"valve\",\"format\":\"deg\",\"value\":\"%03d\"}]}", my_room.settings.Id, str_temp, str_hum, my_room.status.movement, my_room.status.valve_status);
	Serial.println(str);
}
