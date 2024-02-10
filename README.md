# Solar Panel embedded project

<index>
  <summary>Index:</summary>
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
</index>


## Preview of the project
Hi, welcome to our solar panel project for embedded system. \
The idea of this project was to change the way to manage the solar panels.\
Since the controlling of solar panels can be difficult or time consuming, we wanted to garantee a way for a user to check them by remote, where we can give him some information about, simply by using their smartphone or computer. \
In our system we use:
- ***MSP432 P401R***, it's the master device, where, based on some input related to the solar panel and some other conditions, he will decide whether to send a messagge to our user.
- ***ESP32***, we use this microcontroller for the wi-fi module. Obviously useful if we need to communicate with the user about the solar panel status.
- ***Humidity/temperature sensor***, we use this sensor to check and make some controlls about the humidity and temperature in the area close to our solar panels.
- ***Embedded light sensor***, we use this sensor to compare the light with the value returned by our solar panels.
- ***Solar panels***, those are devices we have to check.

The system works by constantly monitoring our solar panels, where based on some pre-registered value we evaluate if it is properly working. \
The pre-register data comes from ideal conditions of the solar panel, such as sunny day, cleaned solar panel etc. Then, by using those data, we compare with what we got from the solar panels and we make the decisions.

For those kind of process, we also consider the weather conditions, since he can critically affect the functionality of the solar panel. In fact, by using temperature and humidity sensor we can figure out if our solar panel isn't working for that reason, or other related conditions.

### Project view
Image of the project view, such as basic graphic view and how the code is divided.

## Implementation
### Used hardware
The main hardware we used is a **MSP432p401r** and a **ESP32**. Those two are the main components of our project, since are the one who process the received data from sensors or external input. \
The MSP432p401r is a ultra-low-power mixed-signal microcontroller, configured with an ARM Cortex-M4 processor. Next we are going to use his integreted lux sensor and the UART connection to communicate with the ESP and the other sensors.
### Software IDE for MSP432
To flash the wanted program into the microcontroller, we need to use a specific software given by the **Texas Instrument Company**, called [Code Composer Studio](https://www.ti.com/tool/download/CCSTUDIO/). This software let us use our microcontroller.

The last process before using our microcontroller, it's to install and include some specific usefull libraries for the implementation of the project. To install those libraries, you can find them [here](https://drive.google.com/file/d/1_5TsECed3wNJpIpllxYYdD06aFbkk7Fc/view) \
To include those libraries you need to insert them inside your project propriety at the following paths:

#### ARM linker
<p align="center">
  <img src="images for embedded/File_search_image.png" width="700">
</p>

#### ARM compiler
<p align="center">
  <img src="images for embedded/Include_libraries.png" width="700">
</p>

