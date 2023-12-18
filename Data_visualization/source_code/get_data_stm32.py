#!/usr/bin/python3
import time
from serial_transmission import *
from influxdb_handler import InfluxHandler


# InfluxDB connection settings
influx_host = "localhost"  # Change this to your InfluxDB host
influx_port = 8086  # Use the same port you mapped when running the InfluxDB container
influx_db = "mydatabase"  # Use the same database name you specified when running the InfluxDB container

influxdb_handler = InfluxHandler(host=influx_host, port=influx_port, database=influx_db)


# open the serial communication with stm32
ser = open_serial_connection()

try:
    # flush input to ensure there is no incomplete data will be received
    ser.flushInput()
    # wait for synchronization
    time.sleep(1)

    while True:
        # send Int_temp  request
        send_request_frame(ser, Int_temp_request)
        print("Int_temp request is sent")
        int_temp = wait_for_response_frame(ser)  # wait for response frame
        if int_temp is None:
            print("int_temp is none")
            ser.flushInput()
        else:
            print("Int_temp is: ", int_temp)
            influxdb_handler.insert_int_temp(int_temp)
        print("\n")

        # send LM35_temp  request
        send_request_frame(ser, LM35_temp_request)
        print("LM35_temp request is sent")
        LM35_temp = wait_for_response_frame(ser)  # wait for response frame
        if LM35_temp is None:
            print("LM35 is none")
            ser.flushInput()
        else:
            print("LM35 is: ", LM35_temp)
            influxdb_handler.insert_LM35_temp(LM35_temp)  # test
        print("\n")

        # send cell_1  request
        send_request_frame(ser, Cell_1_vol_request)
        print("Cell 1 request is sent")
        Cell_1 = wait_for_response_frame(ser)  # wait for response frame
        if Cell_1 is None:
            print("Cell 1 is none")
            ser.flushInput()
        else:
            print("cell 1 is: ", Cell_1)
            influxdb_handler.insert_cell_1_vol(Cell_1)  # test
        print("\n")

        # send cell_2  request
        send_request_frame(ser, Cell_2_vol_request)
        print("Cell 2 request is sent")
        Cell_2 = wait_for_response_frame(ser)  # wait for response frame
        if Cell_2 is None:
            print("Cell 2 is none")
            ser.flushInput()
        else:
            print("cell 2 is: ", Cell_2)
            influxdb_handler.insert_cell_2_vol(Cell_2)
        print("\n")

        # send cell_3  request
        send_request_frame(ser, Cell_3_vol_request)
        print("Cell 3 request is sent")
        Cell_3 = wait_for_response_frame(ser)  # wait for response frame
        if Cell_3 is None:
            print("Cell 3 is none")
            ser.flushInput()
        else:
            print("cell 3 is: ", Cell_3)
            influxdb_handler.insert_cell_3_vol(Cell_3)
        print("\n")

        # send cell_4  request
        send_request_frame(ser, Cell_4_vol_request)
        print("Cell 4 request is sent")
        Cell_4 = wait_for_response_frame(ser)  # wait for response frame
        if Cell_4 is None:
            print("Cell 4 is none")
            ser.flushInput()
        else:
            print("cell 4 is: ", Cell_4)
            influxdb_handler.insert_cell_4_vol(Cell_4)
        print("\n")

        # send shunt  request
        send_request_frame(ser, Shunt_current_request)
        print("Shunt request is sent")
        shunt_current = wait_for_response_frame(ser)  # wait for response frame
        if shunt_current is None:
            print("shunt current is none")
            ser.flushInput()
        else:
            print("shunt current is: ", shunt_current)
            influxdb_handler.insert_current(shunt_current)
        print("\n")

except KeyboardInterrupt:
    print("Script is terminated by user")

except Exception as e:
    print(f"An error occurred: {e}")

finally:
    ser.close()
