import paho.mqtt.client as paho
broker="192.168.0.32"
# broker="127.24.0.4"
port=1883
def on_publish(client,userdata,result):
    print("data published \n")
    pass
client1= paho.Client("control1", protocol=paho.MQTTv311)
client1.on_publish = on_publish
client1.connect(broker,port)
ret= client1.publish("broker/starttime/1","13:37:00", retain=True)
ret= client1.publish("broker/starttime/2","13:39:00", retain=True)
ret= client1.publish("broker/timeout","30", retain=True)