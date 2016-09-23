# [PhyLab 2.0] (https://lizzit.it/phylab)

More details available at https://lizzit.it/phylab

##General description
PhyLab is an open-source, modular system; it makes laboratory experience more interesting simplifying the understanding of science and physics.  
Is has been created for school laboratories; it collects and processes experimental data at an incredibly high rate, makes plots and sends them to connected smartphones and other devices. It is composed of a series of collecting-transmitting Arduino-based units equipped with sensors; these deliver the data to the RaspberryPI-based central unit that produces real-time plots.  
This system collects  different types of data ( temperature, humidity, speed…).

##Hardware
The central unit needs to be a RaspberryPI 1, 2 or 3.  
Each transmitting module sends data to the central unit either via an nRF24L01+ module or via USB.
The data is then saved on the central unit and sent to all the connected clients.  
The central unit can also act as a web server, and as a WiFi hotspot, this allows the system to be transported easily.  
It was designed for physics experiments at school, in a physics laboratory, and from that the name PhyLab 2.0
Each transmitting unit is usually based on an Arduino, connected with a sensor and an nRF24L01+ module but a transmitting unit can be made with any microcontroller and any sensor;  
###PhyLab PCBs
We designed two PhyLab PCBs, one that allows an nRF24L01+ module to be easily connected to the RaspberryPi and one that allows to easily build a transmitting unit; more details are available at https://lizzit.it/phylab/  
<img src="http://i.imgur.com/BDOgZFu.jpg" width=400px/>
<img src="http://i.imgur.com/aqlaFSg.jpg" width=400px/>
<img src="http://i.imgur.com/2D74W3K.jpg" width=400px/>
<img src="http://i.imgur.com/K00j45U.jpg" width=400px/>

##Software
A pre-built image for RaspberryPI 3 is available, it includes all the software required on the server and in addition it creates a WiFi hotspot; this is the easiest way to set up a complete PhyLab 2.0 system.  
It is based on PhyLab OS, a minimal Linux distribution including a kernel build with just the modules required for PhyLab 2.0 to run.  
This allows PhyLab 2.0 to boot in just 5 seconds and to work out of the box with no setup needed.  
It is especially useful during physics lessons because it allows every student to see the plot and download the data from their smartphones.  

Software for some transmitting units we have developed (humidity, temperature, acceleration, distance, voltage and current) will be released soon.  

The pre-built image also includes a DNS and DHCP server (dnsmasq) that allows each device to connect easily and display the chart by going to http://phy.lab/  


##Online demo

Demo version with random data available at: https://lizzit.it/phylab/demo/  
The demo version just shows the web interface, plotting random data, and is not connected to any sensor.
<img src="http://i.imgur.com/mhHIL1G.png" width=200px/>
<img src="http://i.imgur.com/1rWJHnS.png" width=200px/>
<img src="http://i.imgur.com/liXiX2X.png" width=200px/>
<img src="http://i.imgur.com/78djOSZ.png" width=200px/>
<img src="http://i.imgur.com/eIriPYH.png" width=816px/>
