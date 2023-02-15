import socket
import cayenne.client
import time
import subprocess

# Cayenne authentication info obtained from the Cayenne Dashboard.
MQTT_USERNAME  = "e76d14f0-4aaf-11eb-8779-7d56e82df461"
MQTT_PASSWORD  = "d53c13e8374fa923167cec0f7332a702e4ac58af"
MQTT_CLIENT_ID = "4f5d5bb0-4ab0-11eb-b767-3f1a8f1211ba"

# Sink node on the 802.15.4 network
HOST = "zigduino-86.strasbourg.iot-lab.info"

# Connection port
PORT = 20000

# Buffersize to receive data on a socket
BUFSIZE = 4096

# Socket to connect on the serial port of the sink node
socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.connect((HOST, PORT))
print("Connection on the sink node{}".format(PORT))

# The callback for when a message is received from Cayenne.
def on_message(message):
	print("message received: " + str(message))
	# If there is an error processing the message return an error string, otherwise return nothing.

client = cayenne.client.CayenneMQTTClient()
client.on_message = on_message
client.begin(MQTT_USERNAME, MQTT_PASSWORD, MQTT_CLIENT_ID)

i=0

def find_elem(str, line):
	"""Find an element in a line and return the value associated to the element 
	and the consumed element.

	Example: 

	str = from
	line = sink_received;from=0.52;seqno=0;hops=3;len=45;payload=hum=17.1;temp=19.3;light=1023;sound=17;pir=0;

	return value = 0.52, seqno=0;hops=3;len=45;payload=hum=17.1;temp=19.3;light=1023;sound=17;pir=0;


	Parameters:
	----------
	str : string
	The element to find in the line
	line : string
	The line in which to search the element

	Return:
	----------
	the value of the element and the consumed line
	"""
	str_len = len(str)
	idx_decl = line.find(str) 
	line = line[idx_decl + str_len:]

	idx_end = line.find(';')
	value = line[:idx_end]
	line = line[idx_end + 1:]

	print(value, end='\n')
	return value, line

# Array of Zigduino used in this experience
channels = {"0.14" : 1, "0.33" : 6, "0.34" : 11, "0.48" : 16, "0.49" : 21, "0.50" : 26, "0.51" : 31, "0.52" : 36, "0.53" : 41, "0.81": 46}

while True:
	client.loop()

	line = socket.recv(BUFSIZE).decode()
	print(line, end='\n')

	# check if the line contains data
	while (line.find('sink_received') != -1):
		print("sink received detected")
		received_data_idx = line.find('sink_received')
		line = line[received_data_idx + len('sink_received'):]


		# get Zigduino number
		no_zigduino, line = find_elem("from=", line)

		# get humditity
		hum, line = find_elem("hum=", line)

		# get temperature
		temp, line = find_elem("temp=", line)

		# get light
		light, line = find_elem("light=", line)

		# get sound
		sound, line = find_elem("sound=", line)

		# get person in room
		pir, line = find_elem("pir=", line)

		# send data if the Zigduino is not the sink
		if (no_zigduino != "0.86"):
			print("============DATA TO SEND=============")
			print(temp, light, hum, sound, pir, end='\n')

			print(">>>>>>>>>>>Sending data...<<<<<<<<<<<")
	
			client.celsiusWrite(channels[no_zigduino], temp)
			client.luxWrite(channels[no_zigduino] + 1, light)
			client.virtualWrite(channels[no_zigduino] + 2, hum, "rel_hum", "p")
			client.virtualWrite(channels[no_zigduino] + 3, sound, "snr", "db")
			client.virtualWrite(channels[no_zigduino] + 4, pir, "d", "prox")
	
			print("============DATA SEND=============")

	print('---------------------')

