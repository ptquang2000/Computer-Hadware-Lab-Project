SSID = 'Computer Network'
PASSWORD = '1921681251'
HOST = ''
START_TIME = 0
GTM = 7
TIMEOUT = None


def do_connect():
    import network
    global HOST, SSID, PASSWORD
    sta_if = network.WLAN(network.STA_IF)
    if not sta_if.isconnected():
        print('connecting to network...')
        sta_if.active(True)
        sta_if.ifconfig(('192.168.0.32', '255.255.255.0', '192.168.0.1', '192.168.0.1'))
        sta_if.connect(SSID, PASSWORD)
        while not sta_if.isconnected():
            pass
    print('network config:', sta_if.ifconfig())
    HOST =  sta_if.ifconfig()[0]
    import ntptime
    import time
    ntptime.settime()
    try:
        print("Local time after synchronization：%s" %str(time.localtime()))
    except:
        print("Error syncing time")


def check_time_format(time):
    import re
    if re.match('[01]\d:[0-5]\d:[0-5]\d', time):
        return True
    elif re.match('2[0-3]:[0-5]\d:[0-5]\d', time):
        return True
    return False


def sleep_time(start_time):
    import time
    global GTM
    nums = start_time.split(':')
    start_time = int(nums[0]) * 3600 + int(nums[1]) * 60 +  int(nums[2])
    nums = time.localtime()
    local_time = 0
    if int(nums[3]) + GTM < 24:
        local_time = (int(nums[3]) + GTM) * 3600 + int(nums[4]) * 60 +  int(nums[5])
    else:
        local_time = (int(nums[3]) + GTM - 24) * 3600 + int(nums[4]) * 60 +  int(nums[5])
    if start_time > local_time:
        return (start_time - local_time) * 1000
    else:
        return (start_time + 86400 - local_time) * 1000


def esp32_setup():
    import btree
    import json
    with open('topic', 'r+b') as f:
        db = btree.open(f)
        try:
            global TIMEOUT, START_TIME
            i = 1
            start_times = list()
            TIMEOUT = int(json.loads(db[b'broker/timeout'].decode('utf8'))['_app_msg'])
            while True:
                topic = 'broker/starttime/{}'.format(i)
                start_time = json.loads(db[topic.encode()].decode('utf8'))['_app_msg']
                if not check_time_format(start_time):
                    raise Exception('Incorrect START_TIME format')
                start_times.append(start_time)
                i += 1
        except KeyError:
            if i != 1:
                START_TIME = min([sleep_time(start_time) for start_time in start_times])
            else:
                raise OSError
        finally:
            db.close()


def esp32():
    import machine
    import _thread
    import os
    global TIMEOUT, START_TIME, HOST
    _thread.stack_size(16*1024)
    from server import Broker
    broker = Broker(HOST)
    broker.start(None)
    try:
        esp32_setup()
    except OSError:
        print('NO CONFIGURATION')
        TIMEOUT = None
        START_TIME = None
    except Exception as e:
        print(e)
        TIMEOUT = None
        START_TIME = None
    if machine.reset_cause() == machine.DEEPSLEEP:
        print('wake up from deep sleep')
    elif machine.reset_cause() == 3:
        print('remove all last config')
        TIMEOUT = None
        START_TIME = 0
        try:
            os.remove('topic')
        except:
            pass
        try:
            os.remove('session')
        except:
            pass
        try:
            os.remove('log')
        except:
            pass

    print('BROKER TIMEOUT: ', TIMEOUT)
    print('BROKER START TIME AFTER TIMEOUT: ', START_TIME)
    # broker = Broker(HOST)
    if not TIMEOUT:
        print('CONFIG START')
        broker.start(10)
        print('GET CONFIG')
        try:
            esp32_setup()
            print('START DEEPSLEEP FOR: ', START_TIME)
            machine.deepsleep(START_TIME)
        except OSError:
            machine.reset()
    else:
        print('BROKER START')
        broker.start(TIMEOUT)
        print('START DEEPSLEEP FOR: ', START_TIME)
        machine.deepsleep(START_TIME)


def esp8266():
    from umqtt.simple import MQTTClient
    global HOST
    starttime = None
    interval = None
    esp = MQTTClient('valve1', HOST, 1883, keepalive=30)
    esp.connect(clean_session=True)
    esp.subscribe(b'valve1/starttime', qos=1)
    esp.subscribe(b'valve1/interval', qos=1)
    



def main():
    import os
    sysname = os.uname().sysname
    if sysname == 'esp32':
        esp32()
    elif sysname == 'esp8266':
        esp8266()


if __name__ == '__main__':
    do_connect()
    main()