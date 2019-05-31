#! /usr/bin/python3
import time
import serial
import json
import random
import signal
import sys

MESSAGE_LOST_PROB = 0.1
MESSAGE_CORRUPTED_PROB = 0.1
SEND_TIME_DELAY = 0.01	# 10 ms

ser = serial.Serial('/dev/ttyUSB0', 9600)

def signal_handler(sig, frame):
	print('You pressed Ctrl+C!')
	ser.close()
	sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

def room_simulator(Id):
	temp = "{:05.2f}".format(random.uniform(15.0,30.0))
	hum = "{:05.2f}".format(random.uniform(0.0,100.0))
	valve = "{:03d}".format(random.randint(0,100))
	msg = '{"Id":"'+Id+'","Eco":"1","sens":[{"Nm":"Tmp","Val":"'+temp+'","Fmt":"C"},{"Nm":"Hum","Val":"'+hum+'","Fmt":"%"}],"acts":[{"Nm":"Vlv","Val":"'+valve+'","Fmt":"%"}]}'
	guess = random.random()
	if(guess<=MESSAGE_CORRUPTED_PROB):
		print("-----------Corrupted------------")
		c=list(msg)
		c[random.randint(0,len(c)-1)] = '?'
		c[random.randint(0,len(c)-1)] = '?'
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
	while(1):
		ser.flushInput()
		raw_msg = ser.readline().decode('utf-8').rstrip()
		print("--------------------------------")
		print("Receive: " + raw_msg)
		start = raw_msg.split("::")[0]
		if(start=="start"):
			msg=raw_msg.split("::")[1]
			json_msg = json.loads(msg)
			room_simulator(json_msg["Id"])

if __name__ == "__main__":
    main()