# PhyLab 2.0
PhyLab 2.0 - https://lizzit.it/phylab
More details available at https://lizzit.it/phylab

Copernico PhyLAB is an open-source, modular system; it makes laboratory experience more interesting simplifying the understanding of science and physics.
Is has been created for school laboratories; it collects and processes experimental data at an incredibly high rate, makes plots and sends them to connected smartphones and other devices. It is composed of a series of collecting-transmitting Arduino-based units equipped with sensors; these deliver the data to the RaspberryPI-based central unit that produces real-time plots.
This system collects  different types of data ( temperature, humidity, speed…).

The central unit needs to be a RaspberryPI 1, 2 or 3.
Each transmitting module send data to the central unit either via an nRF24L01+ module or via USB.
The data is then saved on the central unit and sent to all the connected clients.
The central unit can also act as a web server, and as a WiFi hotspot, this allows the system to be transported easily.
It was designed for physics experiments at school, in a physics laboratory, and from that the name PhyLab 2.0
A transmitting unit is usually based on an Arduino, connected with a sensor and an nRF24L01+ module but a transmitting unit can be made with any microcontroller and any sensor;
We designed two PhyLab PCBs, one that allows an nRF24L01+ module to be easily connected to the RaspberryPi and one that allows to easily build a transmitting unit; more details are available at https://lizzit.it/phylab/

A pre-built image for RaspberryPI 3 is available, it includes all the software required on the server and in addition it creates a WiFi hotspot; this is the easiest way to set up a complete PhyLab 2.0 system.
It is especially useful during physics lessons because it allows every student to see the plot and download the data from their smartphones.

Software for some transmitting units we have developed (humidity, temperature, acceleration, distance, voltage and current) will be released soon.

Demo version with random data available at: https://lizzit.it/phylab/demo/ The demo version just shows the web interface, plotting random data, and is not connected to any sensor.
