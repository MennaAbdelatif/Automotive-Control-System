# File         : GPS_ACC_integration                                                                               # 
# author       : Menna Abdellatif                                                              #
# Description  : This file gets the accelerometr and GPS readings and parse it                                     #
#                Then send their data to the stm32 microcontroller to take action depend on accelerometr status    #
# version      : 1.0.0                                                                                             #
# date         : 2-15-2023                                                                                         #
####################################################################################################################

import time
import string
import serial
import Adafruit_ADXL345
import paho.mqtt.client as mqtt
import string
import pynmea2
import sys

BROKER = "test.mosquitto.org"      
PORT = 1883
TOPIC_LIVE = "car/location"
TOPIC_ALERT = "car/alert"


# Create an ADXL345 instance.
accel = Adafruit_ADXL345.ADXL345()

#Intialization of UART for recieving data from GPS module and send data to stm32 microcontroller
port="/dev/ttyAMA0"
ser = serial.Serial(
    port,
    baudrate=9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=0.5
)

# Initialize MQTT Client
client = mqtt.Client("CarTracker")
client.connect(BROKER, PORT, 60)
client.loop_start()

# Timing for periodic live location
LIVE_INTERVAL = 15      # seconds between live updates
last_live_time = 0

# Main Loop
while True:
    try:
        newdata = ser.readline()
        if sys.version_info[0] == 3:
            newdata = newdata.decode("utf-8", "ignore")

        # Read accelerometer data
        x, y, z = accel.read()

        # Parse GPS data
        if newdata.startswith("$GPRMC"):
            newmsg = pynmea2.parse(newdata)
            lat = newmsg.latitude
            lng = newmsg.longitude

            # Accident detection (X/Y spikes)
            if abs(x) > 250 or abs(y) > 250:
                alert_msg = f"Accident detected! Latitude: {lat}, Longitude: {lng}"
                print(alert_msg)

                # Publish accident alert via MQTT
				
                client.publish(TOPIC_ALERT, alert_msg)

                # Send formatted GPS string to STM32
				
                gps = f"1{lat}*{lng}#"
                ser.write(gps.encode())
                print("Sent to STM32",gps)
                time.sleep(1)

            # Live location update every LIVE_INTERVAL seconds
			
            if time.time() - last_live_time >= LIVE_INTERVAL:
                live_msg = f"Latitude: {lat}, Longitude: {lng}"
                client.publish(TOPIC_LIVE, live_msg)
                print("Live MQTT update ", live_msg)
                last_live_time = time.time()

        time.sleep(0.5)

    except pynmea2.ParseError:
        continue

    except Exception as e:
        print( "Error ")
        time.sleep(1)
