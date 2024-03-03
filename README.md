# Sunrise-Sunset Automatic Dimmer

## Overview
The Sunrise-Sunset Automatic Dimmer is a microcontroller-based project designed to optimize lighting conditions in large poultry farms through precise dimming control. This system automatically adjusts the light intensity to simulate sunrise and sunset, creating ideal conditions for poultry. It utilizes a custom PCB designed with KiCad, incorporating an ATmega328p microcontroller, a 4n35 optocoupler, and an IRF740 Power MOSFET transistor for seamless light intensity control.

## Project Description

![Alt Text](output.gif)

## Features
- **Automatic Light Dimming**: Simulates sunrise and sunset to provide natural lighting conditions.
- **User-configurable Timers**: Allows setting custom start and end times for the dimming process.
- **Precise Control**: Utilizes a 4n35 optocoupler and an IRF740 Power MOSFET for accurate light intensity adjustments.
- **Custom PCB Design**: Features a professionally designed PCB using KiCad, tailored for easy implementation and durability.
- **ATmega328p Microcontroller**: Offers reliable and efficient control over the dimming process.

## Tools Used
- Arduino IDE
- ATmega328P Microcontroller
- KiCad for PCB Design

## Getting Started
To implement this project, follow the steps below:
1. **PCB Fabrication**: Use the provided KiCad files to fabricate the PCB.
2. **Component Soldering**: Solder the ATmega328p, 4n35 optocoupler, IRF740 Power MOSFET, and other components onto the PCB.
3. **Programming**: Use the Arduino IDE to upload the dimming control software to the ATmega328p microcontroller.
4. **Configuration**: Set the desired start and end times for the dimming process through the user input points.
5. **Installation**: Install the system within the poultry farm setup and connect it to the AC lighting system.

<!--## Configuration & Usage
Detailed instructions on configuring the start and end times for the dimming process, as well as guidelines for optimal usage within a poultry farm environment, are available in the [Configuration Guide](/docs/configuration-guide.md).-->

## Contributors

- Paschalis Moschogiannis (Contact: [pmoschogiannis@uth.gr](mailto:pmoschogiannis@uth.gr))


## License

This project is licensed under the [GNU General Public License Version 3 (GPLv3)](LICENSE).
