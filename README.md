# BATTERY MANAGEMENT SYSTEM 
This project repository contains the firmware for the Battery Management System (BMS), utilizing an `STM32L152RE` microcontroller board to oversee the conditions of four LiFePo4 cells arranged in series, including voltage, current, and temperature. Depending on the monitored conditions, the microcontroller directs the battery current flow into charging-only, discharging-only, or bidirectional modes. A Modbus slave/server protocol was implemented for data communication. A Python script acting as a Modbus Master was created on a Raspberry Pi to serve as a gateway, retrieving status data from the BMS and transmitting it to the cloud for visualization. Grafana was employed as the cloud service for visualization purposes.




## Table of Content  
	
	1. Main Equipment and Components 

	2. Functionalities

	3. Details 

	
## 1. Main Equipments and Components

* `STM32L152RE` (development board)
* `RASPBERRY PI`
* OP AMPS `LM342`
* MOSFET `FDS6898AZ`
* Multiplexer `4051BE`
* Shunt Resistor `WLBR010FE`
* Temperature Sensor `LM35DZ`


## 2. Functionalities

The Battery Management System (BMS) oversees and regulates multiple facets of the battery's condition to ensure both safety and optimal functionality, covering:
* Voltage Monitoring:
	* Tracking the individual cell and theoverall voltage of the battery.
* Temperature Monitoring:
	* Supervise Environment and Battery Pack's temperature.
* Current Supervision:
	* Overseeing the direction and intensity of current flow in and out of the battery. pack.
* Cell Balancing:
	* Maintaining a balanced state among the battery cells.
* Charging and Discharging Management:
	* Regulate MOSFETS to control the charging and discharging states of the battery.

## 3. Details
### Prerequisites
The project requirements: `MOSFET` driven switch (both charging and discharging) Temperature monitoring Individual battery cell voltages STM32 controller with over/under voltage per cell, over/under temperature (charge: 0C to 40C and discharge: -20C to 40C) Communication of cell voltages/temperatures/... over Modbus RTU protocol with the `STM32L152RE` functioning as a Slave and the `Raspberry Pi` serving as the Master.

Presentation of data on Grafana through a Docker container on the Raspberry Pi.


### Project diagram 

![diagram](https://github.com/ThinhLe279/Battery-Management-System/blob/master/Design/BlockDigram.jpg)

### visualization


![display](https://github.com/ThinhLe279/Battery-Management-System/blob/master/Data_visualization/data_grafana.png)
