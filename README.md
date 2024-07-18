## To do

[x] PWM LED
[x] UART Async

### PWM LED

It uses the PWM on the timer `timers1` to run the PWM on the pin PE9 ( alternative function 2).

[x] Check the prj.conf file and enable PWM API.
[x] Check <BOARD>.overlay file and set PWM nodes.
[x] Refactor the code to run PWM LED.

#### UART Async

It uses `usart1` and set it to use with DMA `dma1` with channel 4 as TX and channel 5 as RX.

[x] Check the prj.conf file and enable Serial and UART async APIs.
[x] Check <BOARD>.overlay file and set DMA node `dmas` pins on `usart1` node.
[x] Refactor the code to run UART async.

This project runs for first a test to run correctly a PWM LED and UART Async examples on a STM32F103C8.

# How to build PlatformIO based project

1. [Install PlatformIO Core](https://docs.platformio.org/page/core.html)
2. Download [development platform with examples](https://github.com/platformio/platform-ststm32/archive/develop.zip)
3. Extract ZIP archive
4. Run these commands:

```shell
# Change directory to example
$ cd platform-ststm32/examples/zephyr-blink

# Build project
$ pio run

# Upload firmware
$ pio run --target upload

# Build specific environment
$ pio run -e blackpill_f103c8

# Upload firmware for the specific environment
$ pio run -e blackpill_f103c8 --target upload

# Clean build files
$ pio run --target clean
```
