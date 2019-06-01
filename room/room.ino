#include "DHT.h"
#include <Servo.h>

//#define DEBUG

#define MY_ID 	1

#define TASK_PERIOD 			2000
#define CHECK_MOTION_PERIOD		2000	// depends on the PIR signal
#define CONTROL_VALVE_PERIOD	4000

#define ERROR_LED_PIN 			13
#define MOTION_LED_PIN 			12
#define ENERGY_SAVING_LED_PIN 	11

#define DHT_PIN		10
#define VALVE_PIN	8
#define PIR_PIN		9

#define TEMP_MAX					30
#define TEMP_MIN					15
#define DEFAULT_ENERGY_SAVING_DIFFERENCE	2
#define DEFAULT_DESIRED_TEMPERATURE			24

#define VALVE_CLOSED	20
#define VALVE_LOW		45
#define VALVE_MIDDLE	90
#define VALVE_HIGH		135
#define VALVE_OPEN		160

#define HIGH_VALUE_DIFFERENCE		2
#define APPROACH_TEMP_DIFFERENCE	1

#define ENERGY_SAVING_MOTION_COUNT_THRESHOLD	10
#define MAX_MOTION_COUNT	40

#define MSG_LEN	64
//------------------------------

struct room_settings {
	unsigned int Id;
	unsigned int energy_saving_difference;
	unsigned int desired_temperature;
	unsigned int actual_goal_temperature;
};

struct room_status{
	float temp;
	float hum;
	bool motion;
	unsigned int valve_status;
	bool eco_mode;
	unsigned int motion_count;
};

struct room {
	struct room_settings settings;
	struct room_status status;
};

struct room my_room = {{MY_ID, DEFAULT_ENERGY_SAVING_DIFFERENCE, DEFAULT_DESIRED_TEMPERATURE, DEFAULT_DESIRED_TEMPERATURE},{0,0,0,0,0,0}};

DHT dht(DHT_PIN,DHT22);
Servo valve;

//--------------------------------------

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
	pinMode(MOTION_LED_PIN, OUTPUT);
	// check
	digitalWrite(ERROR_LED_PIN, HIGH);
	delay(200);
	digitalWrite(MOTION_LED_PIN, HIGH);
	delay(200);
	digitalWrite(ENERGY_SAVING_LED_PIN, HIGH);
	delay(200);
	digitalWrite(ERROR_LED_PIN, LOW);
	delay(200);
	digitalWrite(MOTION_LED_PIN, LOW);
	delay(200);
	digitalWrite(ENERGY_SAVING_LED_PIN, LOW);
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

void init_motion() {
	pinMode(PIR_PIN, INPUT);
}

uint8_t str_2d_to_uint8(char* str){
	uint8_t ret = (str[0] - '0')*10;
	ret += (str[1] - '0');
	return ret;
}

uint8_t check_message(char* str){
	uint8_t count_start_graph = 0;
	uint8_t count_end_graph = 0;
	uint8_t count_start_quad = 0;
	uint8_t count_end_quad = 0;
	uint8_t count_quotes = 0;
	uint8_t len = 0;
	while(*str != '\0' && len<MSG_LEN){
		switch(*str){
			case '"':
				count_quotes++;
				break;
			case '{':
				count_start_graph++;
				break;
			case '}':
				count_end_graph++;
				break;
			case '[':
				count_start_quad++;
				break;
			case ']':
				count_end_quad++;
				break;
			default:
				break;
		}
		str++;
		len++;
	}
	if(len>=MSG_LEN)
		return 3;	// message corrupted

	if(count_start_graph > count_end_graph)	// message incomplete
		return 2;

	if(count_start_graph==count_end_graph && count_start_quad==count_end_quad && (count_quotes%2)==0)	// message correct
		return 1;

	return 3;	// message corrupted
}

bool check_json_comand(char* str, uint8_t* id){
	// {"Id":"01","DTemp":"24.00"}
	bool ret = true;
	char tmp[16];

	memcpy(tmp, str+2, 2);
	tmp[2]='\0';
	ret &= (strcmp(tmp, "Id\0")==0);

	memcpy(tmp, str+7, 2);
	tmp[2]='\0';
	*id = str_2d_to_uint8(tmp);

	#ifdef DEBUG
		if(!ret){
			Serial.println("check Json Id fail");
		}
	#endif

	memcpy(tmp, str+12, 5);
	tmp[5]='\0';
	ret &= (strcmp(tmp, "DTemp\0")==0);

	#ifdef DEBUG
		if(!ret){
			Serial.println("check Json DTemp fail");
		}
	#endif

	return ret;
}

bool set_parameters_from_JSON(char* str){
	// {"Id":"01","DTemp":"24.00"}
	bool ret = false;
	char value_str[6];
	float value;
	uint8_t cmd_id = 0;

	ret = check_json_comand(str, &cmd_id);
	if(ret && (cmd_id == MY_ID)){
		memcpy(value_str, str+20, 5);
		value_str[5]='\0';
		value = str_2d_to_uint8(value_str);
		if (value >= TEMP_MIN && value <= TEMP_MAX){
			my_room.settings.desired_temperature = value;
				#ifdef DEBUG
					char str[16], value_str[6];
					dtostrf(my_room.settings.desired_temperature, 4, 2, value_str);
					value_str[5]='\0';
					sprintf(str, "DesTemp: %s", value_str);
					Serial.println(str);
				#endif
			ret = true;
		} else {
			#ifdef DEBUG
				Serial.println("value out of range");
			#endif
			ret = false;
		}
	} else {
		ret = false;
	}
	return ret;
}

bool receive_command(){
	bool ret = false;
	uint8_t check;
	static uint8_t msg_i = 0;
	static char msg[MSG_LEN];

 	while(Serial.available() > 0) {
		#ifdef DEBUG
			digitalWrite(ENERGY_SAVING_LED_PIN, HIGH);
		#endif
		char c = Serial.read();
		if(c != '\n' && c != '\r'){
			msg[msg_i++] = c;
		} else {
			check = check_message(msg);
			msg[msg_i] = '\0';
			msg_i = 0;
			if(check==1){
				if(set_parameters_from_JSON(msg)){
					digitalWrite(ERROR_LED_PIN, LOW);
					ret = true;
				} else {
					digitalWrite(ERROR_LED_PIN, HIGH);
					ret = false;
				}
			} else {
				digitalWrite(ERROR_LED_PIN, HIGH);
				ret = false;
			}
			#ifdef DEBUG
				Serial.print("check: "); Serial.print(check);
				Serial.print(" Received: "); Serial.println(msg);
			#endif
		}
	}
	#ifdef DEBUG
		delay(200);
		digitalWrite(ENERGY_SAVING_LED_PIN, LOW);
	#endif
	return ret;
}

void setup() {
	init_communication();
	init_leds();
	init_motion();
	init_valve();
	init_DHT();
}

void loop() {
	static uint32_t  this_run=0, last_run=0, control_valve_last=0, check_motion_last=0;
	bool check_cmd = false;
	this_run = millis();	

	if(receive_command()){
		send_room_status();
	}

	if((this_run-last_run) >= TASK_PERIOD){
		last_run = this_run;

		dht_sensor_task_run();
		manage_Energy_Saving_mode();

		if((this_run - check_motion_last) >= CHECK_MOTION_PERIOD){
			check_motion_last = this_run;
			check_motion();
		}

		if((this_run - control_valve_last) >= CONTROL_VALVE_PERIOD){
			control_valve_last = this_run;
			control_valve();
			#ifdef DEBUG
				send_room_status();
			#endif
		}
	}
}

void manage_Energy_Saving_mode() {
	if(my_room.status.motion_count < ENERGY_SAVING_MOTION_COUNT_THRESHOLD){
		my_room.settings.actual_goal_temperature = my_room.settings.desired_temperature - my_room.settings.energy_saving_difference;
		my_room.status.eco_mode = true;
		digitalWrite(ENERGY_SAVING_LED_PIN, HIGH);
	} else {
		my_room.settings.actual_goal_temperature = my_room.settings.desired_temperature;
		my_room.status.eco_mode = false;
		digitalWrite(ENERGY_SAVING_LED_PIN, LOW);

	}
}

void check_motion(){ 	// can be implemented as an interrupt on rising/falling edge
	if(digitalRead(PIR_PIN)){
		my_room.status.motion = true;
		digitalWrite(MOTION_LED_PIN, HIGH);
		if(my_room.status.motion_count < MAX_MOTION_COUNT){
			my_room.status.motion_count++;
		}
	} else {
		my_room.status.motion = false;
		digitalWrite(MOTION_LED_PIN, LOW);
		if(my_room.status.motion_count > 0){
			my_room.status.motion_count--;
		}
	}
}

void control_valve(){
	float temp_err_diff = my_room.status.temp - my_room.settings.actual_goal_temperature;
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

uint8_t valve_pos_to_perc(uint8_t pos){
	switch (pos) {
	case VALVE_CLOSED:
		return 0;
		break;
	case VALVE_LOW:
		return 25;
		break;
	case VALVE_MIDDLE:
		return 50;
		break;
	case VALVE_HIGH:
		return 75;
		break;
	case VALVE_OPEN:
		return 100;
		break;
	default:
		return 0;
		break;
	}
}

void send_room_status(){
	char str_temp[6], str_hum[6], str[256];
	uint8_t valve_perc, i=0;
	dtostrf(my_room.status.temp, 4, 2, str_temp); /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
	dtostrf(my_room.status.hum, 4, 2, str_hum);

	valve_perc = valve_pos_to_perc(my_room.status.valve_status);
	sprintf(str, "{\"Id\":\"%02d\",\"Eco\":\"%1d\",\"sens\":[{\"Nm\":\"Tmp\",\"Val\":\"%s\",\"Fmt\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"%s\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"%03d\",\"Fmt\":\"%%\"}]}\n\0", my_room.settings.Id, my_room.status.eco_mode, str_temp, str_hum, valve_perc);
	i = 0;
	while(str[i] != '\0' && i<256){
		Serial.write(str[i]);
		i++;
		delay(25);
	}
//	Serial.print(str);
}
