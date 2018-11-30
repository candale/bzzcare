import os
import sys
import time
import json
import logging
import threading
from queue import Queue
from contextlib import contextmanager

import serial
from serial.serialutil import SerialException
from serial.tools.list_ports_posix import comports


import paho.mqtt.client as mqtt


BAUD_RATE = 115200
MQTT_HOST = 'localhost'
MQTT_USER = 'bzzcare'
MQTT_PASS = 'password'


logger = logging.getLogger(__name__)


class BzzException(Exception):

    pass


class ProtocolException(BzzException):

    pass


def setup_basic_stdout_logging(level='DEBUG', format=None, use_os_stdout=False):
    format = format or '[%(asctime)s %(levelname)s/%(name)s:%(lineno)d] %(message)s'

    logger = logging.getLogger()
    logger.setLevel(level)

    formatter = logging.Formatter(format)
    if use_os_stdout:
        ch = logging.FileHandler('/dev/stdout')
    else:
        ch = logging.StreamHandler()

    ch.setLevel(level)
    ch.setFormatter(formatter)

    logger.addHandler(ch)


class LockedResource:

    def __init__(self, serial_port):
        self.__lock = threading.Lock()
        self.__resource = serial_port

    @contextmanager
    def __call__(self):
        self.__lock.acquire(True)
        yield self.__resource
        self.__lock.release()


def get_serial_port(port_path=None):
    ports = comports()
    if len(ports) > 1 and port_path is None:
        raise SerialException('Multiple ports found. You must specify a target port')

    if len(ports) == 0:
        raise SerialException('No port found')

    found_port_path = None
    if len(ports) == 1:
        if port_path is not None and ports[0].device != port_path:
            raise SerialException('No port {}'.format(port_path))
        found_port_path = ports[0].device
    else:
        for port in ports:
            if port_path is not None and port.device == port_path:
                port_path = port.device
                break

    if port_path is None:
        raise SerialException('No port found')

    serial_port = serial.Serial(port=port_path, baudrate=BAUD_RATE, write_timeout=1)
    # set read non-blocking
    serial_port.timeout = 0
    return serial_port


def make_mqtt_client(hostname, username, password, mqtt_reader_queue):
    client = mqtt.Client(
        client_id='bzzcare_client', protocol=mqtt.MQTTv311)

    client.username_pw_set(username, password)
    client.user_data_set({'queue': mqtt_reader_queue})
    client.on_message = on_mqtt_message
    client.on_connect = on_mqtt_connect

    client.connect(hostname, 1883, 60)

    return client


def on_mqtt_connect(client, userdata, *args):
    logger.info('Connected to MQTT. Subscribing')
    client.subscribe('bzzcare/+/+')


def on_mqtt_message(client, userdata, message):
    # logger.debug('MQTT: {}: {}'.format(message.topic, message.payload))
    topic_data = message.topic.split('/')
    if len(topic_data) != 3:
        logger.error('Got command with bad topic: {}'.format(message.topic))
        return

    _, device, cmd = message.topic.split('/')
    if device.isdigit() is False:
        logger.error('Got invalid device number: {}'.format(device))
        return

    device = int(device)
    if device < 1 or device > 255:
        logger.error(
            'Got invalid device number: {}. Must be between 1 and 255'
            .format(device)
        )
        return

    try:
        payload = json.loads(message.payload.decode())
    except json.JSONDecodeError:
        logger.exception(
            'Could not parse payload. It must be JSON. Payload: {}'
            .format(message.payload)
        )
        return

    userdata['queue'].put(
        {'device': device, 'command': cmd, 'payload': payload})


def pid_mqtt_cmd_praser(command, device, payload):
    payload_bits = [command, str(device)]
    for key, value in payload.items():
        payload_bits.append('{}={}'.format(key, value))

    return ';'.join(payload_bits) + '\n'


PROTOCOL_MQTT_CMD_PARSERS = {
    'pid': pid_mqtt_cmd_praser,
}


def forward_messages_from_mqtt_to_device(message, safe_serial_port=None):
    logger.info("MQTT -> DEVICE: {}".format(message))
    if message['command'] not in PROTOCOL_MQTT_CMD_PARSERS:
        logger.error(
            'Got command from MQTT that is not supported: {}'
            .format(message['command'])
        )

    device_command = PROTOCOL_MQTT_CMD_PARSERS[message['command']](
        message['command'], message['device'], message['payload'])

    with safe_serial_port() as serial_port:
        logger.info(device_command)
        serial_port.write(device_command.encode())


def read_serial_forever(safe_serial_port, queue, protocol_func=None):
    """Read from serial port forever and forward all messages in queue

    :param safe_serial_port: a LockedResource with a serial port in it
    :param queue: a Queue to put the items in after their read an, optionally,
        processed according to protocol_func
    :param protocol_func: a function that takes as a single argument a string
        and produces any result. the result of this function is going to be
        put in the queue
    """
    while True:
        with safe_serial_port() as serial_port:
            line = serial_port.readline()

        if not line:
            time.sleep(0.1)
            continue

        # logger.debug('DEVICE: {}'.format(line))

        if protocol_func is None:
            queue.put(line, block=True, timeout=1)
            continue

        try:
            result = protocol_func(line)
        except Exception as e:
            logger.exception(
                'Failed to run protocol function for line: {}'.format(line))
        else:
            if result is not None:
                queue.put(result, block=True, timeout=1)


def queue_processor(queue, handler, kwargs=None):
    """Get all message from sink, run them though handler

    :param sink: a Queue object where messages can be taken from
    :param handler: a function that takes a single argument, a message
    :param kwargs: extra kwargs that are passed to the handler with each call
    """
    kwargs = kwargs or {}
    while True:
        item = queue.get(block=True)
        try:
            handler(item, **kwargs)
        except Exception as e:
            item_str = 'unknown'
            try:
                item_str = str(item)
            except Exception:
                pass

            logger.exception('Failed to handle message: {}'.format(item_str))
        finally:
            queue.task_done()


def pid_device_cmd_praser(line):
    cmd, device, rssi, *payload_items = line.split(';')
    payload = {'command': cmd, 'device': device, 'rssi': rssi}
    for payload_item in payload_items:
        key, value = payload_item.split('=')
        payload[key] = value

    return payload


PROTOCOL_DEVICE_CMD_PARSERS = {
    'pid': pid_device_cmd_praser,
}


def protocol(line):
    try:
        line = line.strip().decode()
    except UnicodeDecodeError:
        # logger.error('Cannot decode line: {}'.format(line))
        return

    # TODO: figure out more elegantly that a line is a report line
    if line.startswith('pid;') is False:
        return None

    cmd = line.split(';')[0]
    if cmd not in PROTOCOL_DEVICE_CMD_PARSERS:
        logger.error('Got command with no parser: "{}"'.format(cmd))
        return None

    result = PROTOCOL_DEVICE_CMD_PARSERS[cmd](line)
    if not all(['command' in result, 'rssi' in result, 'device' in result]):
        logger.error(
            'All messages must have at least three items: command, rssi and device')

    return result


def forward_message_from_device_to_mqtt(data, mqtt_client=None):
    logger.info("DEVICE -> MQTT: {}".format(data))
    mqtt_client.publish(
        'bzzcare/r/{}/{}'.format(data.pop('device'), data.pop('command')),
        json.dumps(data).encode()
    )


def run():
    setup_basic_stdout_logging()
    serial_port = None
    if len(sys.argv) > 1:
        serial_port = sys.argv[1]

    safe_serial = LockedResource(get_serial_port(port_path=serial_port))
    # queue used for message from device to mqtt
    device_reader_queue = Queue()
    # queue used for message from mqtt to device
    mqtt_reader_queue = Queue()

    device_messages_reader_thread = threading.Thread(
        target=read_serial_forever,
        args=(safe_serial, device_reader_queue),
        kwargs={'protocol_func': protocol},
        daemon=True
    )

    mqtt_messages_processor_thread = threading.Thread(
        target=queue_processor,
        args=(mqtt_reader_queue, forward_messages_from_mqtt_to_device),
        kwargs={'kwargs': {'safe_serial_port': safe_serial}},
        daemon=True
    )

    mqtt_client = make_mqtt_client(
        MQTT_HOST, MQTT_USER, MQTT_PASS, mqtt_reader_queue)
    mqtt_client.loop_start()
    device_messages_processor_thread = threading.Thread(
        target=queue_processor,
        args=(device_reader_queue, forward_message_from_device_to_mqtt),
        kwargs={'kwargs': {'mqtt_client': mqtt_client}},
        daemon=True
    )

    device_messages_reader_thread.start()
    device_messages_processor_thread.start()
    mqtt_messages_processor_thread.start()

    try:
        device_messages_reader_thread.join()
        device_messages_processor_thread.join()
        mqtt_messages_processor_thread.join()
    except KeyboardInterrupt:
        logger.info("Interrupted by user. B-Bye!")
        with safe_serial() as serial_port:
            serial_port.close()


if __name__ == '__main__':
    run()
