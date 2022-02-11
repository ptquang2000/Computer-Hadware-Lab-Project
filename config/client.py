print("Xin chào ThingsBoard")
import paho.mqtt.client as mqttclient
import time
import json

BROKER_ADDRESS = "demo.thingsboard.io"
PORT = 1883


def subscribed(client, userdata, mid, granted_qos):
    print("Subscribed...")


def recv_message(client, userdata, message):
    print("Received: ", message.payload.decode("utf-8"))
    temp_data = {'value': True}
    try:
        jsonobj = json.loads(message.payload)
        if jsonobj['method'] == "setValue":
            temp_data['value'] = jsonobj['params']
            client.publish('v1/devices/me/attributes', json.dumps(temp_data), 1)
    except:
        pass


def connected(client, usedata, flags, rc):
    if rc == 0:
        print("Thingsboard connected successfully!!")
        client.subscribe("v1/devices/me/rpc/request/+")
    else:
        print("Connection is failed")


client = mqttclient.Client("valve1")
client.username_pw_set(username='valve1', password='valve1')

def on_log(client, userdata, level, buf):
    print('log:', buf)

client.on_connect = connected
client.connect(BROKER_ADDRESS, 1883)
client.on_log = on_log
client.loop_start()

client.on_subscribe = subscribed
client.on_message = recv_message

collect_data = {
    'timestart': 1000,
    'interval': 2000,
}

client.subscribe('v1/devices/me/telemetry', 1)
while True:
    time.sleep(10)
    client.publish('v1/devices/me/telemetry', json.dumps(collect_data), 1)

