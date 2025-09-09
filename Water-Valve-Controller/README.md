# Water Valve Controller

## What this folder contains
- `Code/` — Arduino sketch for the valve controller (ESP32)
- `Hardware/Schematics/` — schematic PDFs / EasyEDA project files
- `Hardware/PCB/` — PCB board files and layout
- `Hardware/Gerbers/` — Gerber ZIP for fabrication


## Quick start
1. Open `Code/esp32_valve_controller.ino` in Arduino IDE (ESP32 board support required).
2. Install the libraries listed in `Code/libraries.txt`.
3. Select the correct ESP32 board and flash at 115200 baud.
4. Power system with 12V SMPS or 3S Li-ion pack.

## Notes
- Tested on ESP32-C6 Mini. Adjust pins in the top of the sketch if you use a different module.
