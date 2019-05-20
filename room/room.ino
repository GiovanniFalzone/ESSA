#include "DHT.h"

#define TASK_PERIOD 5000
#define ERROR_LED_PIN 4
#define ENERGY_SAVING_LED_PIN 5
#define GOAL_ACHIEVED_LED_PIN 6

DHT dht(2,DHT22);

void setup() {
	Serial.begin(9600);
	dht.begin();
	pinMode(ERROR_LED_PIN, OUTPUT);
	pinMode(ENERGY_SAVING_LED_PIN, OUTPUT);
	pinMode(GOAL_ACHIEVED_LED_PIN, OUTPUT);
}

void loop() {
	static uint32_t last_run=0, this_run=0;
	this_run = millis();	
	if((this_run-last_run)>=TASK_PERIOD){
		last_run = this_run;
		dht_sensor_task_run();
	}
}

void dht_sensor_task_run() {
	float hum = dht.readHumidity();
	float temp = dht.readTemperature();
	if (isnan(temp) || isnan(hum)) {
		Serial.println("Impossibile leggere il sensore!");
	} else {
		char str_temp[6];
		char str_hum[6];
		dtostrf(temp, 4, 2, str_temp); /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
		dtostrf(hum, 4, 2, str_hum);
		char str[128];
		sprintf(str, "{\"Sensors\":[{\"name\":\"temperature\",\"format\":\"C\",\"value\":\"%s\"},{\"name\":\"humidity\",\"format\":\"%%\",\"value\":\"%s\"}]}", str_temp, str_hum);
		Serial.println(str);
	}
}
