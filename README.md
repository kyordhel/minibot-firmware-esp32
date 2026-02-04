Minibot firmware
================
This repository contains the firmware to operate the minibot.

The frmware is written in C/C++ and relies on the IDF framework
which is based on FreeRTOS. There is an alternate MicroPython version [here](https://github.com/kyordhel/minibot-firmware-esp32-upy).

Directory structure and build based on the template application
to be used with [Espressif IoT Development Framework](https://github.com/espressif/esp-idf).
Do check [ESP-IDF docs](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for getting started instructions.


## Prerrequisites
1. Install required build packages
```bash
sudo apt install git wget flex bison gperf python3 python3-pip python3-setuptools cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
```
2. Download and install Espressif esp-idf version 5.5 inside `~/esp32`
```bash
mkdir ~/esp32
cd ~/esp32
git clone -b v5.5 --depth 1 https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh
```

3. Source the idf. This shall be done **ALWAYS** before attempting to compile an ESP32 project
```bash
source export.sh
```

## Build
**After sourcing** the ESP IDF, execute the following commands:
```bash
cd build
idf.py build -C ..
```

The `idf.py` utility can be run from the directory project, but
that often pollutes the tree with temporaty build files.
To keep our working tree neat and clean we run ir from build.

**IMPORTANT: `idf.py` NOT FOUND**:
I know this would happen. You just forgot to source the IDF, again.


## Flash
**After sourcing** the ESP IDF, execute the following commands:
```bash
idf.py flash
```
