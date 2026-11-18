# AGENTS.md

Firmware for the BrainBoard of a PAMI robot (Coupe de France de Robotique 2027, Robotronik).
ESP32-S3, C++, ESP-IDF v6.0. It is the SPI master to the ControlBoard
([CDFR2027-PAMIs-CODE-Control](https://github.com/robotronik/CDFR2027-PAMIs-CODE-Control)).

## Build & test

```sh
. /opt/esp-idf/export.sh                 # required first
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
cmake -S tests -B tests/build && cmake --build tests/build && ctest --test-dir tests/build   # host tests
```

## Architecture

- `main/`: `app_main` and `board.hpp`, the pin map and single source of truth for pins.
- `components/<name>/`: one IDF component per module, with `include/<name>/` and `src/`.
  Create a component only when its code exists.
- `hello`: a task that prints a line every second. The simplest example of a component, start here.
- `control_link`: SPI master to the ControlBoard + frame protocol. Wire format spec:
  [wiki](https://github.com/robotronik/CDFR2027-PAMIs-CODE-Control/wiki/BrainBoard-SPI-Protocol).
  `protocol.cpp` has no IDF includes, so `tests/` builds it on the host.
  Replies run one exchange behind. A Nop is sent every 50 ms because the ControlBoard
  stops after 500 ms without a valid frame.

## Code conventions

Follow [docs/style.md](docs/style.md).

## Working rules

- Commits go straight to `master`, message style `(type:) summary`
  (e.g. `(feat:)`, `(refactor:)`, `(cleanup:)`). Don't commit `.claude/`.

## Always keep it as simple as possible
