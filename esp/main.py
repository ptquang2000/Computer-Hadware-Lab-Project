SSID = 'Computer Network'
PASSWORD = '1921681251'
HOST = ''
START_TIME = 0
GTM = 7
TIMEOUT = None


def do_connect():
    import network
    sta_if = network.WLAN(network.STA_IF)
    if not sta_if.isconnected():
        print('connecting to network...')
        sta_if.active(True)
        sta_if.ifconfig(('192.168.0.32', '255.255.255.0', '192.168.0.1', '192.168.0.1'))
        sta_if.connect(SSID, PASSWORD)
        while not sta_if.isconnected():
            pass
    print('network config:', sta_if.ifconfig())
    global HOST
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
        except KeyError as e:
            if i != 1:
                START_TIME = min([sleep_time(start_time) for start_time in start_times])
            else:
                raise OSError


def esp32():
    import machine
    import _thread
    _thread.stack_size(16*1024)
    from server import Broker
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
    print('BROKER TIMEOUT: ', TIMEOUT)
    print('BROKER START TIME AFTER TIMEOUT: ', START_TIME)
    broker = Broker(HOST)
    if not TIMEOUT:
        broker.start(10)
        print('Deepsleep indefinitely')
        machine.deepsleep()
    else:
        broker.start(TIMEOUT)
        machine.deepsleep(START_TIME)


def esp8266():
    pass


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