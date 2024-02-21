# Solar Panel embedded project


  <h3>Index:</h3>
  <ol>
    <li><a href="#preview-of-the-project">Preview of the Project</a></li>
      <ul>
        <li><a href="#preview-of-the-project">Introduction</a></li>
      </ul>
      <ul>
        <li><a href="#project-view">Project view</a></li>
      </ul>
    <li><a href="#implementation">Implementation</a></li>
      <ul>
        <li><a href="#preview-of-the-project">Used hardware</a></li>
      </ul>
    <li><a href="#details">Details</a></li>
    <li><a href="#authors">Collaborators</a></li>
    <li><a href="#links">Useful links</a></li>
  </ol>



## Preview of the project
Hi, welcome to our solar panel project for embedded system. \
The idea of this project was to change the way to manage the solar panels.\
Since the controlling of solar panels can be difficult or time consuming, we wanted to garantee a way for a user to check them by remote, where we can give him some information about, simply by using a website on their smartphone or computer. \
In our system we use:
- ***MSP432 P401R***, it's the master device, where, based on some input related to the solar panel and some other conditions, he will decide whether to send a messagge to our user.
- ***ESP32***, we use this microcontroller for the wi-fi module. Obviously useful if we need to communicate with the user about the solar panel status.
- ***DHT20 humidity/temperature sensor***, we use this sensor to check and make some controlls about the humidity and temperature in the area close to our solar panels.
- ***Embedded light sensor***, we use this sensor to compare the light with the value returned by our solar panels.
- ***INA219 power sensor***, we use this sensor to effectivly register the Watt produced by the solar panel.
- ***Solar panels***, those are devices we have to check.
- ***MQTT Server***, is a light-weight messaging protocol used to communicate between machines, using a centralized server.

The system works by constantly monitoring our solar panels, where based on some pre-registered value we evaluate if it is properly working. \
The pre-register data comes from ideal conditions of the solar panel, such as sunny day, cleaned solar panel etc. Then, by using those data, we compare with what we got from the solar panels and we make the decisions.

For those kind of process, we also consider the weather conditions, since he can critically affect the functionality of the solar panel. In fact, by using temperature, humidity and brightness sensor we can figure out if our solar panel isn't working for that reason, or other related conditions.

### Project view
<p align="center">
  <img src="images for embedded/init_project_files.png" width="400">
</p>

```
Solar Panel project:
├── ccsProject
│   ├── main.c                     # Main code of the MSP432
│   └── Hardware                   # We initialize the hardware of our devices
|       ├── hardware_init          # Initialize the sensors and MSP432 modules
|       ├── Graphic                # Graphic part of the MSP432
|       ├── LcdDriver              # Drivers of the MSP432 LCD
|       └── luxDriver              # Drivers of the integrated MSP432 lux sensor
├── ESP32
│   ├── main.ino                   # Main code of the ESP32
│   ├── Wifi                       # Include libraries for the Wifi connection
│   ├── PubSubClient               # Include libraries for MQTT connection
├── serverProject
│   ├── BlogicDB.java              # It handle the databases operations
│   ├── mqtt-client.js             # Useful functions for the web-application
│   ├── mqttws31.js                # Main library of the MQTT server
├── Read_me_Images
│   └── images                     # Images for the Readme file
└── README.md
```


### Used hardware
The main hardware we used is a **MSP432p401r** and a **ESP32**. Those two are the main components of our project, since are the one who process the received data from sensors or external input. \
The MSP432p401r is a ultra-low-power mixed-signal microcontroller, configured with an ARM Cortex-M4 processor. Next we are going to use his integreted lux sensor and the UART connection to communicate with the ESP and the other sensors.
### Software IDE for MSP432
To flash the wanted program into the microcontroller, we need to use a specific software given by the **Texas Instrument Company**, called [Code Composer Studio](https://www.ti.com/tool/download/CCSTUDIO/). This software let us use our microcontroller.

The last process before using our microcontroller, it's to install and include some specific usefull libraries for the implementation of the project. To install those libraries, you can find them [here](https://drive.google.com/file/d/1_5TsECed3wNJpIpllxYYdD06aFbkk7Fc/view). \
To include those libraries you need to insert them inside your project propriety at the following paths:

#### ARM linker
<p align="center">
  <img src="images for embedded/File_search_image.png" width="700">
</p>

#### ARM compiler
<p align="center">
  <img src="images for embedded/Include_libraries.png" width="700">
</p>

# Tools configurations:

Follow the schema below in the picture and connect all the sensors and microcontrollers to each other. \
Try to keep the same ports and pins number, since they are fondamental for the functioning of the project:
<p align="center">
  <img src="images for embedded/launch_debug.png" width="500">
</p>


## MSP432 initialization:
When you create your CCS project with all the related files inside our folder `folder CCS`, make sure to implement all the libraries of [before](#software-ide-for-msp432).
After that, you need to flash the memory into the MSP432 by launching this command in the CCS program: 

<p align="center">
  <img src="images for embedded/launch_debug.png" height="100">
</p>

After uploading the flash inside the MSP432, you should see a screen like that appearing on your LCD:

<p align="center">
  <img src="images for embedded/LCD_example.jpg" height="300">
</p>

## ESP32 initialization:

First, if you haven't done it yet, you need to install the Arduino IDE, you can find the link [here](https://www.arduino.cc/en/software) . \
You need also the library for the MQTT communication: 

<p align="center">
  <img src="images for embedded/PubSubClient lib.png" width="300">
</p>

We created a specific folder for the ESP32 code `Nome della cartella codice Arduino ESP32` , where in the `code.ino` you can find the code you need to flash in the ESP32 memory.\
In ` code.ino ` you have to modify the following code:

```
// Wi-Fi setting
const char* ssid = "/* WiFi name */";
const char* password = "/* WiFi password */";

// MQTT setting
const char* ipServer = "/* MQTT server IP */";
const int mqttPort = 1883;                // default server communication port
const char* mqttUser = "MPS432";          // casual name
const char* mqttPassword = "";            // left it empty it's not request
```

After finding the file, you need to connect the ESP32 to your computer and flash the program into the ESP32 memory with this command:
<p align="center">
  <img src="images for embedded/launch_debug_arduino_debug.png" width="500">
</p>

With the code you have just flashed, we can manage to connect with two of our devices:
* MQTT server, so we can send and receive data from our server.
* ESP32 UART connection to our MSP432, so we can send and get information about our connected sensors.

## Initialization of the MQTT server
After doing so, we install the components for initialize the MQTT server. You need to follow those steps:

  <li>You can find the link to download it: https://mosquitto.org/download/ </li>
  <li>Open the command line as administrator.</li>
  <li>
    After opening the command line, you need to go to the mosquito directory, the folder where the mosquito download is.
  </li>
  <p align="center">
    <img src="images for embedded/command_line_cd.png" width="300">
  </p>
  <li>
    Then, we need to start the server with this command:
  </li>
  
  ``` 
    mosquitto -v -c mosquitto.conf 
  ```

  <li>
    If you have correctly followed all the step of before, you should see those kind of replies:
  </li>
  <p align="center">
    <img src="images for embedded/server_example_show.jpg" width="600">
  </p>

  After that, each communication that pass throught the server could be read on this command line. The information that will be shown are:
  * star/end connection of a Client
  * subscribtion to a topic for a Client
  * message sending(topic name, payload weight, sender)


  ## Creation of the Web App
We used VSCode to create a web application to implement some features, such as:
* change the sensor reading frequency
* view table for last registered sensor data
* get data from a Postgres database


If you don't know how to create a web-application, these links can help you:
- ***[Create Maven project](https://code.visualstudio.com/docs/java/java-build)***,
- ***[Add server](https://code.visualstudio.com/docs/java/java-tomcat-jetty)***, we used Apache-Tomcat-9.0.41

Once you built your project with Maven and add the demployment on the Tomcat server, you can reach your web-app at the link: www.localhost:8080/solar_panel_webapp/


Web-Application project structure:
```
SOLAR_PANEL_WEBAPP:
├── src\main
│   ├── java\web
│   │   ├── BLogicDB.java
│   │   └── GetinMax.java
│   └── webapp
|       ├── css
|       |   └── style.css
|       ├── js
|       |   ├── mqtt-client.js
|       |   └── mqttws31.js
|       ├── WEB-INF
|       |   └── web.xml
│       └── index.jsp
├── pom.xml
└── README.txt
```

# Authors

* [Matteo Frigo](https://github.com/FrigoMatteo)
  * Telegram bot
  * NodeJS server
  * ESP32 code 
* [Quentin Meneghini](https://github.com/kenny2817)
  * UART communication
  * Light and temperature sensors
  * Electrical wiring
* [Alberto Messa](https://github.com/MessaAlberto)
  * Aplication interface
  * Soil moisture sensor mapping
  * Code testing

## Links

* [Video](https://youtu.be/mkZK5LPMyHU)
* [Project presentation](https://github.com/dongi01/SmartIrrigation_IoTUniTN/blob/64b6fd78eb5f5494780a2737d49178241027ac5e/IoT%20Project%20Presentation.pptx)


