# esp32-transceiver-for_RC_boat
This project aims to create fully functional transmitter and receiver, for RC application, using ESP32 chip with Long Range ESPNOW.

## Specifications
### The remote controller
The remote controller should embed :
- An ESP32 (as main microcontroller)
- A screen (probably a ST7789)
- Some joysticks and buttons
- A bunch of leds

The RC-boat should embed :
- An ESP32 (as main microcontroller)
- An ESC and motor
- A servo motor
- A GPS (probably the NEO-6M-0-001 on a GY-NEO6MV2 module)
- A Compass (probably the HMC5583L module)
- A bunch of buttons and leds for setup

### Wireless link :
The wireless link will be handled by the ESP32s, using a Long Range version of ESPNOW.

I've tested that using the embedded antenna on the ESP32 module DEVKITv1, the range is about 100m ~ 150m on clear sight. Which is enough for me now.

My goal is then to use the embedded GPS/Compass to have autonomous degrees of freedom.

## Contact
If you are interested in this project and have any questions, please feel free to contact me ! (:

I do not claim to be an expert with ESP32, but I've worked quite a bit with ESP-IDF toolchain.

It took me a while to understand everything I know about this powerful toolchain,
thus I will be happy to help beginners using this in their projects. ;)
