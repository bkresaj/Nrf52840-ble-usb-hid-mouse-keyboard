#!/usr/bin/env python3

import serial
import queue
import multiprocessing
import argparse

parser = argparse.ArgumentParser(description="Python translation of your script")
parser.add_argument("--port", required=True, help="Specify the port")

def data_receiver(serial_port, data_queue):
    with serial.Serial(serial_port, 115200, timeout=1) as ser:
        print(f"Data receiver started for {serial_port}.")
        try:
            while True:
                line = ser.readline().decode("utf-8", errors="ignore")
                data_queue.put(line.strip())
        except serial.SerialException:
            print(f"Error: Could not open {serial_port} or connection lost.")
        except KeyboardInterrupt:
            print("Data receiver stopped.")


def data_processor(data_queue):
    while True:
        try:
            data = data_queue.get(timeout=1)
            print(data)
        except queue.Empty:
            pass


if __name__ == "__main__":
    args = parser.parse_args()

    serial_port = args.port

    data_queue = multiprocessing.Queue()

    receiver_process = multiprocessing.Process(
        target=data_receiver, args=(serial_port, data_queue)
    )
    receiver_process.daemon = (
        True 
    )
    receiver_process.start()

    data_processor(data_queue)