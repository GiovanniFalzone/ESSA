#! /usr/bin/python3
import time
import serial
import json
import random
import signal
import sys

MESSAGE_LOST_PROB = 0.1
MESSAGE_CORRUPTED_PROB = 0.1
SEND_TIME_DELAY = 0.02
MANY_CORRUPT = 3

ser = serial.Serial('/dev/ttyUSB0', 9600)

def signal_handler(sig, frame):
	print('You pressed Ctrl+C!')
	ser.close()
	sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

def room_simulator(Id):
	temp = "{:05.2f}".format(random.uniform(15.0,30.0))
	hum = "{:06.2f}".format(random.uniform(0.0,100.0))
	valve = "{:03d}".format(random.randint(0,100))
	eco = str(random.randint(0,1))
	msg = '{"Id":"'+Id+'","Eco":"'+eco+'","sens":[{"Nm":"Tmp","Val":"'+temp+'","Fmt":"C"},{"Nm":"Hum","Val":"'+hum+'","Fmt":"%"}],"acts":[{"Nm":"Vlv","Val":"'+valve+'","Fmt":"%"}]}'
	guess = random.random()
	if(guess<=MESSAGE_CORRUPTED_PROB):
		print("-----------Corrupted------------")
		c=list(msg)
		for j in range(0,MANY_CORRUPT):
			c[random.randint(0,len(c)-1)] = '?'
		msg = "".join(c)

	guess = random.random()
	if(guess<=MESSAGE_LOST_PROB):
		print("Message lost")
	else:
		print("Send: "+msg)
		for i in msg:
			ser.write(bytes(i, 'utf-8'))
			time.sleep(SEND_TIME_DELAY)
		ser.write(bytes('\n', 'utf-8'))

def main():
	ser.flushInput()
	raw_msg = ser.readline().decode('utf-8').rstrip()
	print("--------------------------------")
	print(raw_msg)
	while(1):
		ser.flushInput()
		raw_msg = ser.readline().decode('utf-8').rstrip()
		print("--------------------------------")
		print("Receive: " + raw_msg)
		try:
			json_msg = json.loads(raw_msg)
#			if(json_msg["Id"] == "02"):
			room_simulator(json_msg["Id"])
		except ValueError as e:
			print("JSON not compliant")

if __name__ == "__main__":
    main()