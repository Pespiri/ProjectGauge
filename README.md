# __Project:Gauge__

[![format](https://github.com/Pespiri/ProjectGauge/actions/workflows/clang-format.yaml/badge.svg)](https://github.com/Pespiri/ProjectGauge/actions/workflows/clang-format.yaml)

__`Project:Gauge`__ is an Arduino based CAN bus project using an [ATmega328P](https://www.microchip.com/wwwproducts/ATmega328p) to drive an automotive gauge stepper motor.

## __Development__

### __Hardware__

  * Arduino Uno
  * MCP2515 CAN bus controller (the 16MHz version)
  * x27.168 (Automotive Gauge Stepper Motor )

### __Getting started__

1. Install [Visual Studio Code](https://code.visualstudio.com/) and launch the editor
1. Install the [PlatformIO](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide) extension
1. Go to 'PlatformIO->Platforms' and install `Atmel AVR`
1. Go to the PlatformIO Home Screen and select `Open Project`
1. Open a new terminal or git bash from the project folder and run the following commands:
    * `git submodule init`
    * `git submodule sync`
    * `git submodule update`
