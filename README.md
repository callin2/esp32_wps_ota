ESP32 ARDUINO WPS OTA
=====================

- original code : https://github.com/SensorsIot/ESP32-OTA
- rel. video : https://www.youtube.com/watch?v=1pwqS_NUG7Q

# what I do
- add wps feature.

# need to prepare
- install and setup platformio. https://platformio.org/
- try esp_wps example.
- try arduino ota example.

# how to use
## setup
- compile and upload sketch once.
- open serial monitor program.
- press WPS button on your wifi device.
- remember IP address of your esp32. if no error then you can find esp32's IP adress i your serial monitor program.
- uncomment several lines in `platformio.ini` 

## enjoy cable free development with ota
- you only need to supply power to your eps32.
- add your code.
- compile and upload via ota. 
