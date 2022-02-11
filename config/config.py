from time import sleep
import paho.mqtt.client as paho
broker="192.168.0.32"
# broker="127.24.0.4"
port=1883
def on_publish(client,userdata,result):
    print("data published \n")
    pass
def on_log(client, userdata, level, buf):
    print("log: ",buf)
client1= paho.Client("control1", protocol=paho.MQTTv311)
client1.on_publish = on_publish
client1.on_log = on_log
client1.connect(broker,port)
ret= client1.publish("broker/starttime/1","16:50:00", retain=True, qos=1)
ret= client1.publish("broker/starttime/2","07:30:00", retain=True, qos=1)
ret= client1.publish("broker/timeout","7200", retain=True, qos=1)
ret= client1.publish("valve1/starttime","14:26:30", retain=True, qos=1)
ret= client1.publish("valve1/interval","30", retain=True, qos=1)
client1.loop_forever()