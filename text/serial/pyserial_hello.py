#!/usr/bin/python 
# coding=UTF-8 
import serial
import time
if __name__ == '__main__': 
	serial = serial.Serial("/dev/ttyUSB0", 115200) 
	print serial
	if serial.isOpen():
		print("open success") 
	else: 
		print("open failed")
	data = serial.read(1) 
	try: 
		while True: 
			count = serial.inWaiting() 
			if count > 0: 
				data = serial.read(count) 
			if data != b'': 
				print("receive:", data) 
				serial.write(data) 	
			else: 
				serial.write(hexsend(data)) 
	except KeyboardInterrupt: 
		if serial != None: 
			serial.close()  
