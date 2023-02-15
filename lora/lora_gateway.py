# coding: utf-8

import socket
import time

# Buffersize to receive data on a socket
BUFSIZE = 4096

# The acknowledge number 
nb_ack = 0

# LoRa node
HOST_LORA = "zigduino-83.strasbourg.iot-lab.info"

# Sink node on the 802.15.4 network
HOST = "zigduino-14.strasbourg.iot-lab.info"

# Connection port
PORT = 20000

# Socket to connect on the serial port of the LoRa node
socketL = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socketL.connect((HOST_LORA, PORT))
print("Connection on the LoRa node {}".format(PORT))

# Socket to connect on the serial port of the sink node
socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.connect((HOST, PORT))
print("Connection on the sink node{}".format(PORT))

def wait_signal():
    """Wait for a signal to be received."""
    global nb_ack 
    nb_ack = nb_ack + 1

    while True:
        recv_data = socketL.recv(BUFSIZE).decode()
        if (int(recv_data) == nb_ack):
            print("Wait finished")
            break

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
    
    return value, line


while True:
    line = socket.recv(BUFSIZE).decode()
    print(line, end='\n')
    nb_data = 0

    # check if the line contains line
    received_data_idx = line.find('sink_received')
    if (received_data_idx != -1):
        nb_data = line.count('sink_received')
        socketL.send(bytes([nb_data]))

        wait_signal()

    for i in range(nb_data):
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
        sound, line = find_elem("light=", line)
        
        # get sound
        light, line = find_elem("sound=", line)
        
        # get person in room
        pir, line = find_elem("pir=", line)
        
        print("============DATA TO SEND=============")
        data = no_zigduino[2:] + " " + hum + " " + temp + " " + sound + " " + light + " " + pir
        print(data, end='\n')
        print(">>>>>>>>>>>Sending data...<<<<<<<<<<<")
        socketL.send(data.encode())
        time.sleep(5)
        line_recv = socketL.recv(BUFSIZE).decode()
        print(line_recv, end='\n')
        print("============DATA SEND=============")
        
        wait_signal()
            
    print('---------------------')
    
    time.sleep(1)
    line = ""
    nb_ack = 0

print("Close")
socket.close()
socketL.close()
