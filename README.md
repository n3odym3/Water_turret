# Water_turret

This water turret uses a WemoD1, two 9g servo motors, a Nema17 stepper motor (+DRV8825 driver) and 3D printed parts to create a water turret that can be remotely controlled via MQTT and automatically aims and shoot water at different locations.

## Control
The robot can be controlled by sending simple JSON commands :
- X axis : {"X": 0 to 180}
- Y axis : {"Y": 0 to 180}
- Pump : {"P" : steps (200 steps/rotation)}
Ex : {"X": 30, "Y":50} then {"P": 10000}

## NodeRed demo
The demo on NodeRed (installed on a Raspberry Pi zero) allows to test and remote control the motors
Sensors and advances automation sequences could be added to the flow to water several plants depending on soil moisture or add a camera with motion detection to make an anti-intrusion system or anything else.
 
