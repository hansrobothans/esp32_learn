Thanks [pcbreflux](https://github.com/pcbreflux) provide code for arduino IDE: [ESP32 IR Control](https://github.com/pcbreflux/espressif/tree/master/esp32/arduino/sketchbook/ESP32_IR_Remote)

### How to use it:

**Setup:** *IRrecvInit*(pin, port)

- pin: pin number of ESP32.
- port: chanel of remote control (0 - 7).

**Read IR code**: *IRrecvReadIR*()

  Read IR code from remote.
