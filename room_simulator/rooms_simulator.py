#! /usr/bin/python3
import time
import serial
import json
import random
import signal
import sys
import _thread

NOISE = True

MESSAGE_LOST_PROB = 0.1
MESSAGE_CORRUPTED_PROB = 0.1
DELAY_SEND_BYTE = 0.02
SEND_INIT_DELAY = 2
COM_DEADLINE = 60
MANY_CORRUPT = 3

rooms = [ '04']
rooms_request_time = [ 0]

ser = serial.Serial('/dev/ttyUSB0', 9600)

def signal_handler(sig, frame):
	print('You pressed Ctrl+C!')
	ser.close()
	sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

def check_com_deadline(threadName):
	while(1):
		for i in range(0, len(rooms_request_time)):
			if (int(time.time() - rooms_request_time[i])>COM_DEADLINE):
				print('------------Room: ' + rooms[i] + ' COM_DEADLINE expired--------------')
				room_step(rooms[i])
				time.sleep(5)
		time.sleep(COM_DEADLINE)

def init_rooms():
	for i in range(0, len(rooms)):
		rooms_request_time[i] = int(time.time())
		print('Init room: ' + rooms[i])
		room_step(rooms[i])
		time.sleep(SEND_INIT_DELAY)

def room_step(Id):
	temp = "{:05.2f}".format(random.uniform(15.0,30.0))
	hum = "{:06.2f}".format(random.uniform(0.0,100.0))
	valve = "{:03d}".format(random.randint(0,100))
	eco = str(random.randint(0,1))
	msg = '{"Id":"'+Id+'","Eco":"'+eco+'","sens":[{"Nm":"Tmp","Val":"'+temp+'","Fmt":"C"},{"Nm":"Hum","Val":"'+hum+'","Fmt":"%"}],"acts":[{"Nm":"Vlv","Val":"'+valve+'","Fmt":"%"}]}'
	guess = random.random()
	if(guess<=MESSAGE_CORRUPTED_PROB and NOISE):
		print("-----------Corrupted------------")
		c=list(msg)
		for j in range(0,MANY_CORRUPT):
			c[random.randint(0,len(c)-1)] = '?'
		msg = "".join(c)

	guess = random.random()
	if(guess<=MESSAGE_LOST_PROB and NOISE):
		print("Message lost")
	else:
		print("Send: "+msg)
		for i in msg:
			ser.write(bytes(i, 'utf-8'))
			time.sleep(DELAY_SEND_BYTE)
		ser.write(bytes('\n', 'utf-8'))

def main():
	ser.flushInput()
	init_rooms()
	try:
		_thread.start_new_thread( check_com_deadline, ("Thread-check_COM_DEADLINE", ))
	except:
		print("Error: unable to start thread")

	while(1):
		ser.flushInput()
		raw_msg = ser.readline().decode('utf-8').rstrip()
		print("--------------------------------")
		print("Receive: " + raw_msg)
		try:
			json_msg = json.loads(raw_msg)
			if(json_msg["Id"] in rooms):
				i = rooms.index(json_msg["Id"])
				rooms_request_time[i] = int(time.time())
				room_step(json_msg["Id"])
		except ValueError as e:
			print("JSON not compliant")

if __name__ == "__main__":
    main()