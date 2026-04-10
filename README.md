<h1 align="center">
  downshift
  <br>
  <img src="https://github.com/alsy4/COM3505_YYY_Assignment1/blob/main/other/nooodle.png" alt="downshift logo" title="downshift logo" width="300">
  <br>
</h1>
<p align="center" style="font-size: 1.2rem;">A funtional and better helper to make sure that your noodle always cook perfect.</p>

## The idea

The main idea of this project is to have a functional device that would be to detect the change in temperature around the stove. This device reflects the main feature of Internet of Things by helping the user to detect the temperature of the stove remotely from the mobile phone or laptop in real time change.

## The Example Setting

There will be 4 situations that would be trigger 4 different patterns. For this project, we choose the situation where we cook instant noodle in the kitchen.

### LED Pattern
1. Blinking Effect - It is the default setting for the device where all the three LEDs would blinking together at the same time. This would show that the kitchen/stove is at the normal and safe temperature.
2. Chase (Triggered after temperature pass the threshold level, will be occured for 5 seconds, then it will trigger Emergency mode) - 5 second is quite relevant because the stove can be at threshold temperature for that long before damage happen to the kitchen or house
3. Random Flicker (Emergency mode) - This pattern would be triggered after the kitchen/stove has been at above the threshold temperate for a certain time. This would be able to alert the user to turn off or turn down their stove. To simulate urgency, the pattern make sure user feel the urgency to turn off the stove
4. No LED light - indicate that device is not working/inactive 

## How to use this PerfectNoodle

This device uses the ESP32 Wifi to connect to the server. Go to the `Assignment1/src/wifi.cpp` to change the wifi credentials and server information. Make sure that you laptop are also connected to the wifi before doing this:

```
// ---------------- Wi-Fi credentials ----------------
const char* wifiSsid     = "YOUR WIFI NAME";
const char* wifiPassword = "YOUR WIFI PASSWORD";

// ---------------- Server information ----------------
const char* serverHost = "YOUR IP ADDRESS";
const int   serverPort = 9194;
```

To find your IP Address of your computer,
Windows - run `ipconfig` on command prompt. You can find it on `IPv4 Address`.
MAC - run `ifconfig` or `ip addr` on terminal.

## How to run flask server

1. You need to install flask first if you have not done so.

```
pip install flask
```

2. Navigaate to the flask folder. `Assigment1/flask_server`.

```
cd .\Assignment1\flask_server\
```

3. Run the server

```
python server.py
```

## Upload code into ESP32
Remember to Bootload your ESP32 by holding `Boot` and pressing `Reset` button first.

## Open the web browser
Now, you can see the webpage in the browser through this: [text](http://localhost:9194)
