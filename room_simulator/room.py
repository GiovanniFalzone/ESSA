#! /usr/bin/python3
import time
import serial
import signal
import sys
def signal_handler(sig, frame):
        print('You pressed Ctrl+C!')
        sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

with serial.Serial('/dev/ttyUSB0', 9600) as ser:
	while(1):
		ser.flushInput()
		msg = ser.readline().decode('utf-8').rstrip()
		print("Receive: "+msg)
		if(msg == 'ciao!'):
			msg = '{"Id":"01","Eco":"1","sens":[{"Nm":"Tmp","Val":"24.00","Fmt":"C"},{"Nm":"Hum","Val":"56.78","Fmt":"%"}]}'
			for i in msg:
				ser.write(bytes(i, 'utf-8'))
				time.sleep(0.1)
