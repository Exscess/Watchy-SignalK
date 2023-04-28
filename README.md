# Watchy-SignalK
Example project to consume and show data from Signal K json source. Based on [Watchy_GSR](https://github.com/GuruSR/Watchy_GSR) project.

### Instruments:
* COG:      navigation.courseOverGroundTrue
* DPT:      environment.depth.belowTransducer
* BTR V:    electrical.batteries.1.voltage
* BTR A:    electrical.batteries.1.current

### Modes
There are three modes of work:
* NS - no Signal K updates.
* SS - slow Signal K updates. Can be turned on/off by pressing CANCEL and UP buttons at the same time. Do update on start and then after every minute.
* FS - no SignalK updates. Can be turned on/off by pressing CANCEL and DOWN buttons at the same time. Do update on start and then in every cycle of wifi connect. Consuming battery and unstable atm.

### Use
There are not pre-built firmware, so to use it you will have to install platform.io and run
`pio run --target upload; pio device monitor` from main directory. Firmware will be uploaded to connected Watchy.

As mentioned before, project based on top of [Watchy_GSR](https://github.com/GuruSR/Watchy_GSR). Menu and all other functions are still available. Please follow the link to see how to setup Wi-Fi and other settings.

SignalK json endpoint is set in the code:
`String signalKUrl = "https://demo.signalk.org/signalk/v1/api/vessels/urn:mrn:signalk:uuid:a6eb6272-6715-4dcc-9d44-350be66bbe8a";`
Change it to your sailboat endpoint or keep it as it is for development purpose.

### More
![Alt text](img/IMG_5894.jpg?raw=true "One")
![Alt text](img/IMG_5895.jpg?raw=true "Two")