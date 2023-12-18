from influxdb import InfluxDBClient
import time


class InfluxHandler:
    def __init__(self, host, port, database):
        self.client = InfluxDBClient(host=host, port=port)
        self.client.switch_database(database)

    def insert_int_temp(self, int_temp):
        json_body = [
            {
                "measurement": "BMS_measurement",
                "tags": {},
                "time": time.ctime(),
                "fields": {"Internal_temp": int_temp},
            }
        ]
        self.client.write_points(json_body)

    def insert_LM35_temp(self, LM35_temp):
        json_body = [
            {
                "measurement": "BMS_measurement",
                "tags": {},
                "time": time.ctime(),
                "fields": {"LM35_temp": LM35_temp},
            }
        ]
        self.client.write_points(json_body)

    def insert_current(self, current):
        json_body = [
            {
                "measurement": "BMS_measurement",
                "tags": {},
                "time": time.ctime(),
                "fields": {"current": current},
            }
        ]
        self.client.write_points(json_body)

    def insert_cell_1_vol(self, voltage):
        json_body = [
            {
                "measurement": "BMS_measurement",
                "tags": {},
                "time": time.ctime(),
                "fields": {"cell_1_vol": voltage},
            }
        ]
        self.client.write_points(json_body)

    def insert_cell_2_vol(self, voltage):
        json_body = [
            {
                "measurement": "BMS_measurement",
                "tags": {},
                "time": time.ctime(),
                "fields": {"cell_2_vol": voltage},
            }
        ]
        self.client.write_points(json_body)

    def insert_cell_3_vol(self, voltage):
        json_body = [
            {
                "measurement": "BMS_measurement",
                "tags": {},
                "time": time.ctime(),
                "fields": {"cell_3_vol": voltage},
            }
        ]
        self.client.write_points(json_body)

    def insert_cell_4_vol(self, voltage):
        json_body = [
            {
                "measurement": "BMS_measurement",
                "tags": {},
                "time": time.ctime(),
                "fields": {"cell_4_vol": voltage},
            }
        ]
        self.client.write_points(json_body)
