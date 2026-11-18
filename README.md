# CDFR2027-PAMIs-CODE-Brain

Firmware for the BrainBoard (ESP32-S3) of Robotronik's 2027 PAMI robots (Coupe de France de Robotique).
It is the SPI master to the ControlBoard, which runs [CDFR2027-PAMIs-CODE-Control](https://github.com/robotronik/CDFR2027-PAMIs-CODE-Control).
Wire format: [BrainBoard SPI Protocol](https://github.com/robotronik/CDFR2027-PAMIs-CODE-Control/wiki/BrainBoard-SPI-Protocol).

The pins are placeholders for now (TODO in `main/board.hpp`).

## Build & flash

```sh
. /opt/esp-idf/export.sh
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
```

## Host tests

```sh
cmake -S tests -B tests/build && cmake --build tests/build && ctest --test-dir tests/build
```

## LSP

After a build: `ln -s build/compile_commands.json .`
