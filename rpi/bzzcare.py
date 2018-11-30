import os
import sys
import time
import logging
from queue import Queue
from contextlib import contextmanager

import serial
from serial.serialutil import SerialException
from serial.tools.list_ports_posix import comports

import threading


BAUD_RATE = 115200


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



def serial_read_forever(safe_serial_port, queue, protocol_func=None):
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


def queue_processor(queue, handler):
    """Get all message from sink, run them though handler

    :param sink: a Queue object where messages can be taken from
    :param handler: a function that takes a single argument, a message
    """
    while True:
        item = queue.get(block=True)
        try:
            handler(item)
        except Exception as e:
            item_str = 'unknown'
            try:
                item_str = str(item)
            except Exception:
                pass

            logger.exception('Failed to handle message: {}'.format(item_str))
        finally:
            queue.task_done()


def pid_cmd_praser(line):
    cmd, device, rssi, *payload_items = line.split(';')
    payload = {'command': cmd, 'device': device, 'rssi': rssi}
    for payload_item in payload_items:
        key, value = payload_item.split('=')
        payload[key] = value

    return payload


PROTOCOL_CMD_PARSERS = {
    'pid': pid_cmd_praser,
}


def protocol(line):
    try:
        line = line.strip().decode()
    except UnicodeDecodeError:
        logger.error('Cannot decode line: {}'.format(line))
        return

    # TODO: figure out more elegantly that a line is a report line
    if line.startswith('pid;') is False:
        return None

    cmd = line.split(';')[0]
    if cmd not in PROTOCOL_CMD_PARSERS:
        logger.error('Got command with no parser: "{}"'.format(cmd))
        return None

    result = PROTOCOL_CMD_PARSERS[cmd](line)
    if not all(['command' in result, 'rssi' in result, 'device' in result]):
        logger.error(
            'All messages must have at least three items: command, rssi and device')

    return result


def forward_to_mqqt(data):
    print(data)


def run():
    setup_basic_stdout_logging()
    serial_port = None
    if len(sys.argv) > 1:
        serial_port = sys.argv[1]

    safe_serial = LockedResource(get_serial_port(port_path=serial_port))
    queue = Queue()
    reader_thread = threading.Thread(
        target=serial_read_forever,
        args=(safe_serial, queue),
        kwargs={'protocol_func': protocol},
        daemon=True
    )
    processor_thread = threading.Thread(
        target=queue_processor, args=(queue, forward_to_mqqt), daemon=True)

    reader_thread.start()
    processor_thread.start()

    try:
        reader_thread.join()
        processor_thread.join()
    except KeyboardInterrupt:
        logger.info("Interrupted by user. B-Bye!")
        with safe_serial() as serial_port:
            serial_port.close()


if __name__ == '__main__':
    run()
