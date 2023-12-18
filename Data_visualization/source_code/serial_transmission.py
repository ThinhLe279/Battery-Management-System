import serial
import struct


# request frames
Int_temp_request = bytes([0x04, 0x04, 0x00, 0x01, 0x00, 0x01, 0x60, 0x5F])
LM35_temp_request = bytes([0x04, 0x04, 0x00, 0x02, 0x00, 0x01, 0x90, 0x5F])
Cell_1_vol_request = bytes([0x04, 0x04, 0x00, 0x03, 0x00, 0x01, 0xC1, 0x9F])
Cell_2_vol_request = bytes([0x04, 0x04, 0x00, 0x04, 0x00, 0x01, 0x70, 0x5E])
Cell_3_vol_request = bytes([0x04, 0x04, 0x00, 0x05, 0x00, 0x01, 0x21, 0x9E])
Cell_4_vol_request = bytes([0x04, 0x04, 0x00, 0x06, 0x00, 0x01, 0xD1, 0x9E])
Shunt_current_request = bytes([0x04, 0x04, 0x00, 0x07, 0x00, 0x01, 0x80, 0x5E])


# this function is used for open the serial connection between Rasberry Pi and STM32 board
def open_serial_connection():
    ser = serial.Serial(
        "/dev/ttyACM0", 115200
    )  # replace this by your baundrate and port
    return ser


def send_request_frame(ser, request_frame):
    ser.write(request_frame)


def wait_for_response_frame(ser):
    response = b""
    response += ser.read(7)
    print("received frame: ", response.hex())

    if response[0] != 0x04:
        print("wrong slave, exit function....")
        return None

    print("slave addr is correct, check CRC...")
    check_CRC = CRC16(response).hex()

    if check_CRC != "0000":
        print("wrong CRC, exit function")
        return None
    print("CRC is Okay, returning data....")
    data = parse_response_frame(response)
    return data


def parse_response_frame(response_frame):
    # Extract data from response frame
    value = struct.unpack("!h", response_frame[3:5])[0]
    return value / 10.0


def CRC16(nData):
    crc = 0xFFFF
    for byte in nData:
        crc ^= byte
        for _ in range(8):
            if crc & 1:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc = crc >> 1
    return crc.to_bytes(2, byteorder="little")
