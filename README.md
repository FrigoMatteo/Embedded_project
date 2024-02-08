# Solar Panel embedded project

<index>
  <h3>Index:</h3>
  <ol>
    <li><a href="#preview-of-the-project">Preview of the Project</a></li>
    <li><a href="#project-layout">Graphical view of the project</a></li>
    <li><a href="#hardware_used">Hardware used</a></li>
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
- MSP432 P401R, it's the master device, where, based on some input related to the solar panel and some other conditions, he will decide whether to send a messagge to our user.
- ESP32, we use this microcontroller for the wi-fi module. Obviously useful if we need to communicate with the user about the solar panel status.
- Humidity/temperature sensor, we use this sensor to check and make some controlls about the humidity and temperature in the area close to our solar panels.
- Embedded light sensor, we use this sensor to compare the light with the value returned by our solar panels.
- Solar panel, it's the device we have to check.


