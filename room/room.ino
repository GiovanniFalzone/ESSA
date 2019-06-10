#include "DHT.h"
#include <Servo.h>

//#define DEBUG
//-----------------------
#define DELAY_SEND_CHAR	10

#define MY_ID 	1

#define TASK_PERIOD 			2000	// ms
#define CONTROL_VALVE_PERIOD	4000	// ms

#define HIGH_VALUE_DIFFERENCE		2	// ind C°
#define APPROACH_TEMP_DIFFERENCE	1	// ind C°

#define COM_ERROR_DEADLINE			60000	// in ms

#define MOTION_TIME_SLOT			30000	// in ms

#define TEMP_MAX					30
#define TEMP_MIN					15

#define HUM_MAX					100
#define HUM_MIN					0

#define DEFAULT_ENERGYSAVING_OFFSET	2
#define DEFAULT_DESIRED_TEMPERATURE			24

#define MSG_REQUEST_LEN	64


#define ERROR_LED_PIN 			13
#define MOTION_LED_PIN 			12
#define ENERGYSAVING_LED_PIN 	11

#define DHT_PIN		10
#define VALVE_PIN	8
#define PIR_PIN		9

#define VALVE_CLOSED_PIN	6
#define VALVE_OPEN_PIN		7

#define VALVE_CLOSED	10
#define VALVE_LOW		45
#define VALVE_MIDDLE	90
#define VALVE_HIGH		135
#define VALVE_OPEN		170

#define SYSTEM_ERRORS	3
#define SENSOR_ERROR	0
#define VALVE_ERROR		1
#define COM_ERROR		2

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
	unsigned long last_motion_time;
};

struct room {
	struct room_settings settings;
	struct room_status status;
};

struct valve_settings{
	unsigned int open_pos;
	unsigned int closed_pos;
	unsigned int middle_pos;
	unsigned int low_pos;
	unsigned int high_pos;
};

struct valve_settings valve_settings = {VALVE_OPEN, VALVE_CLOSED, VALVE_MIDDLE, VALVE_LOW, VALVE_HIGH};

struct room my_room = {{MY_ID, DEFAULT_ENERGYSAVING_OFFSET, DEFAULT_DESIRED_TEMPERATURE, DEFAULT_DESIRED_TEMPERATURE},{0,0,0,0,0,0}};

DHT dht(DHT_PIN,DHT22);
Servo valve;

// DHT error, Valve error, Communication error
bool systems_errors [SYSTEM_ERRORS];

//--------------------------------------

void signal_led(uint8_t pin){
	digitalWrite(pin, LOW);
	delay(200);
	digitalWrite(pin, HIGH);
	delay(200);
	digitalWrite(pin, LOW);
}

void move_valve(unsigned int pos){
	valve.write(pos);
	delay(15);
	my_room.status.valve_status = pos;
	systems_errors[VALVE_ERROR] = false;

	if(pos == valve_settings.open_pos || pos == valve_settings.closed_pos){
		delay(1000);	//wait to reach the position
		if((pos == valve_settings.open_pos) && (!digitalRead(VALVE_OPEN_PIN))){
			systems_errors[VALVE_ERROR] = true;
		}
		if((pos == valve_settings.closed_pos) && (!digitalRead(VALVE_CLOSED_PIN))){
			systems_errors[VALVE_ERROR] = true;
		}
	}
}

void init_valve() {
	unsigned int pos = 0;

	systems_errors[VALVE_ERROR] = false;

	pinMode(VALVE_CLOSED_PIN, INPUT);
	pinMode(VALVE_OPEN_PIN, INPUT);
	valve.attach(VALVE_PIN);

	move_valve(VALVE_MIDDLE);
	delay(1000);
	for(pos=VALVE_MIDDLE; pos<=VALVE_OPEN; pos++){
		move_valve(pos);
		if(digitalRead(VALVE_OPEN_PIN)){
			valve_settings.open_pos = pos;
			digitalWrite(ERROR_LED_PIN, HIGH);
			delay(200);
			digitalWrite(ERROR_LED_PIN, LOW);
			break;
		}
	}

	move_valve(VALVE_MIDDLE);
	delay(1000);
	for(pos=VALVE_MIDDLE; pos>=VALVE_CLOSED; pos--){
		move_valve(pos);
		if(digitalRead(VALVE_CLOSED_PIN)){
			valve_settings.closed_pos = pos;
			digitalWrite(ERROR_LED_PIN, HIGH);
			delay(200);
			digitalWrite(ERROR_LED_PIN, LOW);
			break;
		}
	}
	valve_settings.middle_pos = valve_settings.closed_pos + (valve_settings.open_pos - valve_settings.closed_pos)*0.5;
	valve_settings.high_pos = valve_settings.middle_pos + (valve_settings.open_pos - valve_settings.middle_pos)*0.5;
	valve_settings.low_pos = valve_settings.closed_pos + (valve_settings.middle_pos - valve_settings.closed_pos)*0.5;

	#ifdef DEBUG
	char str[50];
	sprintf(str, "o:%d, h:%d, m:%d, l:%d, c:%d", valve_settings.open_pos, valve_settings.high_pos, valve_settings.middle_pos, valve_settings.low_pos, valve_settings.closed_pos);
	Serial.println(str);
	#endif


	if((valve_settings.middle_pos >= valve_settings.open_pos) || (valve_settings.middle_pos <= valve_settings.closed_pos)){
		digitalWrite(ERROR_LED_PIN, HIGH);
		while(1);
	}
	if((valve_settings.high_pos <= valve_settings.middle_pos) || (valve_settings.high_pos >= valve_settings.open_pos)){
		digitalWrite(ERROR_LED_PIN, HIGH);
		while(1);
	}
	if((valve_settings.low_pos >= valve_settings.middle_pos) || (valve_settings.low_pos <= valve_settings.closed_pos)){
		digitalWrite(ERROR_LED_PIN, HIGH);
		while(1);
	}

	move_valve(valve_settings.open_pos);
	delay(1000);
	move_valve(valve_settings.high_pos);
	delay(1000);
	move_valve(valve_settings.middle_pos);
	delay(1000);
	move_valve(valve_settings.low_pos);
	delay(1000);
	move_valve(valve_settings.closed_pos);
	delay(1000);
}

void init_leds(){
	pinMode(ERROR_LED_PIN, OUTPUT);
	pinMode(ENERGYSAVING_LED_PIN, OUTPUT);
	pinMode(MOTION_LED_PIN, OUTPUT);
	// check
	digitalWrite(ERROR_LED_PIN, HIGH);
	delay(200);
	digitalWrite(MOTION_LED_PIN, HIGH);
	delay(200);
	digitalWrite(ENERGYSAVING_LED_PIN, HIGH);
	delay(200);
	digitalWrite(ERROR_LED_PIN, LOW);
	delay(200);
	digitalWrite(MOTION_LED_PIN, LOW);
	delay(200);
	digitalWrite(ENERGYSAVING_LED_PIN, LOW);
}

void init_DHT(){
	systems_errors[SENSOR_ERROR] = false;
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
	delay(TASK_PERIOD);
}

void init_communication(){
	Serial.begin(9600);
	systems_errors[COM_ERROR] = false;
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
	while(*str != '\0' && len<MSG_REQUEST_LEN){
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
	if(len>=MSG_REQUEST_LEN)
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
	static char msg[MSG_REQUEST_LEN];

 	while(Serial.available() > 0) {
		char c = Serial.read();
		if(c != '\n' && c != '\r'){
			msg[msg_i++] = c;
		} else {
			check = check_message(msg);
			msg[msg_i] = '\0';
			msg_i = 0;
			if(check==1){
				if(set_parameters_from_JSON(msg)){
					systems_errors[COM_ERROR] = false;
					ret = true;
					#ifdef DEBUG
						signal_led(ENERGYSAVING_LED_PIN);
					#endif
				} else {
					ret = false;
				}
			} else {
				ret = false;
			}
			#ifdef DEBUG
				Serial.print("check: "); Serial.print(check);
				Serial.print(" Received: "); Serial.println(msg);
			#endif
		}
	}
	return ret;
}

void manage_error_led(){
	unsigned int i=0;
	bool error = false;
	for(i=0; i<SYSTEM_ERRORS; i++){
		error |= systems_errors[i];
	}
	digitalWrite(ERROR_LED_PIN, error);

	if(systems_errors[VALVE_ERROR]){
		init_valve();
	}

	if(systems_errors[SENSOR_ERROR]){
		init_DHT();
	}

	#ifdef DEBUG
		char str[50];
		sprintf(str, "E_dht:%d, E_com:%d, E_vlv:%d", systems_errors[SENSOR_ERROR], systems_errors[COM_ERROR], systems_errors[VALVE_ERROR]);
		Serial.println(str);
		delay(200);
	#endif
}

void setup() {
	uint8_t i = 0;
	for(i=0; i<SYSTEM_ERRORS; i++){
		systems_errors[i] = false;
	}
	init_communication();
	#ifdef DEBUG
		Serial.println("-------setup start--------");
	#endif
	init_DHT();
	init_leds();
	init_motion();
	init_valve();
	#ifdef DEBUG
		Serial.println("-------setup end--------");
	#endif
}

void loop() {
	static uint32_t  this_run = 0, last_run = 0, control_valve_last = 0, last_message_time = 0;
	bool check_cmd = false;
	this_run = millis();	

//------------------------------------
	if((this_run-last_run) >= TASK_PERIOD){
		last_run = this_run;
		manage_error_led();
		dht_sensor_task_run();
		check_motion();
		manage_Energy_Saving_mode();
	}

//------------------------------------
	if((this_run - control_valve_last) >= CONTROL_VALVE_PERIOD){
		control_valve_last = this_run;
		control_valve();
	}

//----------background task-----------
	if(receive_command()){
		last_message_time = this_run;
		send_room_status();
	} else {
		if((this_run-last_message_time) > COM_ERROR_DEADLINE){
			#ifdef DEBUG
				Serial.println("----------Communication error, no message received-----------");
			#endif
			systems_errors[COM_ERROR] = true;
			last_message_time = this_run;
			send_room_status();
		}
	}
//------------------------------------
}

void manage_Energy_Saving_mode() {
	if((unsigned long)(millis() - my_room.status.last_motion_time) >= MOTION_TIME_SLOT){
		my_room.settings.actual_goal_temperature = my_room.settings.desired_temperature - my_room.settings.energy_saving_difference;
		if(my_room.settings.actual_goal_temperature < TEMP_MIN){
			my_room.settings.actual_goal_temperature = TEMP_MIN;
		}
		my_room.status.eco_mode = true;
		digitalWrite(ENERGYSAVING_LED_PIN, HIGH);
	} else {
		my_room.settings.actual_goal_temperature = my_room.settings.desired_temperature;
		my_room.status.eco_mode = false;
		digitalWrite(ENERGYSAVING_LED_PIN, LOW);
	}
}

void check_motion(){
	if(digitalRead(PIR_PIN)){
		my_room.status.motion = true;
		digitalWrite(MOTION_LED_PIN, HIGH);
		my_room.status.last_motion_time = millis();
	} else {
		my_room.status.motion = false;
		digitalWrite(MOTION_LED_PIN, LOW);
	}
}

void control_valve(){
	float temp_err_diff = my_room.status.temp - my_room.settings.actual_goal_temperature;
	// low temperature
	if(temp_err_diff < -HIGH_VALUE_DIFFERENCE){
		move_valve(valve_settings.open_pos);
	} else if(temp_err_diff < -APPROACH_TEMP_DIFFERENCE){
		move_valve(valve_settings.high_pos);
	}

	// high temperature
	if(temp_err_diff > HIGH_VALUE_DIFFERENCE){
		move_valve(valve_settings.closed_pos);
	} else if (temp_err_diff > APPROACH_TEMP_DIFFERENCE){
		move_valve(valve_settings.low_pos);
	}

	if((temp_err_diff >= -APPROACH_TEMP_DIFFERENCE) &&  (temp_err_diff <= APPROACH_TEMP_DIFFERENCE)){
		move_valve(valve_settings.middle_pos);
	}
}

bool dht_sensor_task_run() {
	float hum = dht.readHumidity();
	float temp = dht.readTemperature();
	if (isnan(temp) || isnan(hum) || (temp < TEMP_MIN) || (temp > TEMP_MAX) || (hum < HUM_MIN) || (hum > HUM_MAX)) {
		#ifdef DEBUG
			Serial.println("Impossibile leggere il sensore!");
		#endif
		systems_errors[SENSOR_ERROR] = true;
		return false;
	} else {
		systems_errors[SENSOR_ERROR] = false;
		my_room.status.hum = hum;
		my_room.status.temp = temp;
		return true;
	}
}

uint8_t valve_pos_to_perc(uint8_t pos){
	if(pos == valve_settings.closed_pos) return 0;
	if(pos == valve_settings.low_pos) return 25;
	if(pos == valve_settings.middle_pos) return 50;
	if(pos == valve_settings.high_pos) return 75;
	if(pos == valve_settings.open_pos) return 100;
}

void send_room_status(){
	char str_temp[6], str_hum[7], str[256];
	uint8_t valve_perc, i=0;

	dtostrf(my_room.status.temp, 5, 2, str_temp);
	for(i=0;i<6;i++){
		if(str_temp[i]==' ') str_temp[i]='0';
	}
	dtostrf(my_room.status.hum, 6, 2, str_hum);
	for(i=0;i<7;i++){
		if(str_hum[i]==' ') str_hum[i]='0';
	}

	valve_perc = valve_pos_to_perc(my_room.status.valve_status);
	sprintf(str, "{\"Id\":\"%02d\",\"Eco\":\"%1d\",\"sens\":[{\"Nm\":\"Tmp\",\"Val\":\"%s\",\"Fmt\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"%s\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"%03d\",\"Fmt\":\"%%\"}]}\n\0", my_room.settings.Id, my_room.status.eco_mode, str_temp, str_hum, valve_perc);
	if(DELAY_SEND_CHAR == 0){
		Serial.print(str);
	} else {
		i = 0;
		while(str[i] != '\0' && i<256){
			Serial.write(str[i]);
			i++;
			delay(DELAY_SEND_CHAR);
		}
	}
}
