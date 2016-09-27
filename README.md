# [PhyLab 2.0] (https://lizzit.it/phylab)

More details available at https://lizzit.it/phylab

##General description
PhyLab is an open-source, modular system; it makes laboratory experience more interesting simplifying the understanding of science and physics.  
Is has been created for school laboratories; it collects and processes experimental data at an incredibly high rate, makes plots and sends them to connected smartphones and other devices. It is composed of a series of collecting-transmitting Arduino-based units equipped with sensors; these deliver the data to the RaspberryPI-based main unit that produces real-time plots.  
This system collects  different types of data ( temperature, humidity, speed…).

##Hardware
The main unit needs to be a RaspberryPI 1, 2 or 3.  
Each transmitting module sends data to the main unit either via an nRF24L01+ module or via USB.
The data is then saved on the main unit and sent to all the connected clients.  
The main unit can also act as a web server, and as a WiFi hotspot, this allows the system to be transported easily.  
It was designed for physics experiments at school, in a physics laboratory, and from that the name PhyLab 2.0
Each transmitting unit is usually based on an Arduino, connected with a sensor and an nRF24L01+ module but a transmitting unit can be made with any microcontroller and any sensor;  
###PhyLab PCBs
We designed two PhyLab PCBs, one that allows an nRF24L01+ module to be easily connected to the RaspberryPi and one that allows to easily build a transmitting unit; more details are available at https://lizzit.it/phylab/  
<img src="http://i.imgur.com/BDOgZFu.jpg" width=400px/>
<img src="http://i.imgur.com/aqlaFSg.jpg" width=400px/>
<img src="http://i.imgur.com/2D74W3K.jpg" width=400px/>
<img src="http://i.imgur.com/K00j45U.jpg" width=400px/>

###Build it yourself
####Main unit
For the main unit a RaspberryPI and an nRF24L01+ module are required; a DS1307 RTC is highly recommended since it allows the system to work even without wired internet connection; a reliable time, obtained either via NTP or via an RTC, is fundamental for the system to properly store data on the internal memory.  
  
The easiest way to build the main unit is to use a PhyLab 2.0 RaspberryPI module; it allows the nRF24L01+ to be easily connected to the RaspberryPI with no additional wiring needed.  
  
Building a unit without a PhyLab 2.0 RaspberryPI module is also possible.  
The RTC module shall be connected to I2C pins on the P1 header of the RaspberryPI.  
The nRF24L01+ module shall be connected to the RaspberryPI as shown on the diagram under /pcb; adding capacitor between VCC and GND may be useful to smooth current peaks of the nRF24L01+, which may cause system crashes.  
PhyLab 2.0 RaspberryPI module already includes capacitors between VCC and GND lines.  
####Transmitting units
The easiest way to build a transmitting unit is to use a PhyLab 2.0 experiment module but a trasmitting unit can be also built easily with an Arduino, an nRF24L01+ module and a sensor.  
The nRF24L01+ shall be wired to the Arduino as follows:
| nRF24L01+ pin  | Arduino pin |
| ------------- | ------------- |
| CE  | pin 8 |
| CSN  | pin 7 |
| MOSI  | pin 11  |
| MISO  | pin 12  |
| SCK  | pin 13  |
| IRQ  | N/C |
| VCC  | 3.3v  |
| GND  | GND  |

##Software
A pre-built image for RaspberryPI 3 is available, it includes all the software required on the server and in addition it creates a WiFi hotspot; this is the easiest way to set up a complete PhyLab 2.0 system.  
It is based on PhyLab OS, a minimal Linux distribution including a kernel build with just the modules required for PhyLab 2.0 to run.  
This allows PhyLab 2.0 to boot in just 5 seconds and to work out of the box with no setup needed.  
It is especially useful during physics lessons because it allows every student to see the plot and download the data from their smartphones.  

The pre-built image also includes a DNS and DHCP server (dnsmasq) that allows each device to connect easily and display the chart by going to http://phy.lab/  

Software for some transmitting units we have developed (humidity, temperature, acceleration, distance, voltage and current) will be released soon.  

Software for some experiments we have developed (humidity, acceleration and voltage) has been released, it can be uploaded either on a PhyLab 2.0 module or on an Arduino board.  

##Online demo

Demo version with random data available at: https://lizzit.it/phylab/demo/  
The demo version just shows the web interface, plotting random data, and is not connected to any sensor.
<img src="http://i.imgur.com/mhHIL1G.png" width=200px/>
<img src="http://i.imgur.com/1rWJHnS.png" width=200px/>
<img src="http://i.imgur.com/liXiX2X.png" width=200px/>
<img src="http://i.imgur.com/78djOSZ.png" width=200px/>
<img src="http://i.imgur.com/eIriPYH.png" width=816px/>
