SSID = 'Computer Network'
PASSWORD = '1921681251'
HOST = ''
START_TIME = ''
GTM = 7
TIMEOUT = None


def do_connect():
    import network
    sta_if = network.WLAN(network.STA_IF)
    if not sta_if.isconnected():
        print('connecting to network...')
        sta_if.active(True)
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


def sleep_time(time):
    import time
    nums = time.split(':')
    start_time = int(nums[0]) * 3600 + int(nums[1]) * 60 +  int(nums[2])
    nums = time.localtime()
    local_time = (int(nums[3]) + GTM) * 3600 + int(nums[4]) * 60 +  int(nums[5])
    return (local_time - start_time) * 1000


def setup():
    try:
        import btree
        import json
        with open('topic', 'r+b') as f:
            db = btree.open(f)
            global TIMEOUT, START_TIME
            TIMEOUT = int(json.loads(db[b'sys/timeout'].decode('utf8'))['_app_msg'])
            START_TIME = json.loads(db[b'sys/starttime'].decode('utf8'))['_app_msg']
            if not check_time_format(START_TIME):
                raise Exception('Incorrect START_TIME format')
    except OSError:
        print('NO CONFIG FILE')
    except KeyError:
        print('NO CONFIG VALUE')
    except Exception as e:
        print(e)


def main():
    from server import Broker
    broker = Broker()
    broker.start(TIMEOUT)
    if START_TIME:
        import machine
        machine.deepsleep(sleep_time(START_TIME))

    

if __name__ == '__main__':
    do_connect()
    setup()
    # main()