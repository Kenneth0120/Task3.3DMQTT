import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO
import time

# GPIO setup
led1_pin = 17  # GPIO pin where the LED is connected
led2_pin = 27
GPIO.setmode(GPIO.BCM)  # Use Broadcom pin numbering
GPIO.setup(led1_pin, GPIO.OUT)
GPIO.setup(led2_pin, GPIO.OUT)

# MQTT Broker details
broker = "broker.emqx.io"
port = 1883
topic = "SIT210/wave"

# Callback when connecting to the MQTT broker
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT Broker!")
        client.subscribe(topic)  # Subscribe to the topic on successful connect
    else:
        print(f"Failed to connect, return code {rc}\n")

# Callback when receiving a message from the MQTT broker
def on_message(client, userdata, msg):
    print(f"Received '{msg.payload.decode()}' from '{msg.topic}' topic")
    message = msg.payload.decode()
    
    if message == "Wave detected!":blink_led(led1_pin, 3, 0.5)  # Blink the LED 3 times when a message is received
    
    elif message == "Pat detected!":blink_led(led2_pin, 5, 1)
	
def blink_led(pin, blink_count, delay_time):
    for _ in range(blink_count):
        GPIO.output(pin, GPIO.HIGH)  # Turn LED on
        time.sleep(delay_time)  # Wait for 'delay_time' seconds
        GPIO.output(pin, GPIO.LOW)  # Turn LED off
        time.sleep(delay_time)  # Wait for 'delay_time' seconds

# Set up client instance
client = mqtt.Client()

# Bind callback functions
client.on_connect = on_connect
client.on_message = on_message

try:
    # Connect to the broker
    client.connect(broker, port, 60)

    # Blocking call that processes network traffic, dispatches callbacks, and handles reconnecting.
    client.loop_forever()
except KeyboardInterrupt:
    print("Script interrupted by user")
finally:
    GPIO.cleanup()  # Clean up GPIO on normal exit
