# Rotating Cornhole Board Project

## Introduction
Welcome to the Rotating Cornhole Board project! This project utilizes an Arduino Nano microcontroller along with TMC2209 stepper motor drivers, Nema 17 stepper motors, and a giant 3D printed lazy susan bearing to create a dynamically rotating cornhole board. With this setup, the hole on the cornhole board moves, adding an exciting twist to the classic game of cornhole.

## Features
- Battery-powered operation for portability and convenience.
- Offers multiple operating modes:
  - **Constant Speed Mode**: Maintains a consistent rotation speed.
  - **Position Hold Mode**: Changes the position of the hole incrementally by pressing the up button.
  - **Chaos Mode**: Randomizes speed, direction, and distance spun for added excitement.
- Precise control using stepper motors for accurate positioning.
- Features an Arduino-based control system for easy integration and modification.
- User-friendly interface with dedicated buttons:
  - **Mode Button**: Switch between operating modes.
  - **Speed Up Button**: Increase rotation speed.
  - **Speed Down Button**: Decrease rotation speed.
  - **Power Button**: Turn the system on/off.

## Components
- Arduino Nano: The brain of the system, responsible for controlling the stepper motors and coordinating the rotation of the cornhole board.
- TMC2209 Stepper Motor Drivers: These drivers provide smooth and silent operation of the stepper motors, ensuring precise movement.
- Nema 17 Stepper Motors: These motors drive the rotation of the cornhole board.
- Giant 3D Printed Lazy Susan Bearing: Provides a sturdy and smooth rotating platform for the cornhole board.

## Setup
1. **Assembly**: Begin by assembling the cornhole board and attaching the giant 3D printed lazy susan bearing to the underside of the board.
2. **Wiring**: Connect the stepper motors to the TMC2209 stepper motor drivers, and then connect the drivers to the Arduino Nano according to the provided pinout.
3. **Programming**: Upload the provided Arduino sketch to the Arduino Nano using the Arduino IDE or your preferred method.
4. **Power**: Connect an appropriate power source to the Arduino Nano and the stepper motor drivers.

## Usage
1. Power on the system.
2. Use the provided controls (buttons, switches, or a user interface) to specify the desired direction and speed of rotation.
3. Enjoy playing cornhole with the added challenge of a moving target!

## Notes
- Ensure that the stepper motors are securely attached to the cornhole board to prevent slippage during rotation.
- Be cautious when operating the rotating cornhole board, especially around children or pets.
- Adjust the speed and direction of rotation according to the skill level and preferences of the players.

## Contributing
Contributions to the project are welcome! If you have any ideas for improvements or new features, feel free to fork the repository, make your changes, and submit a pull request.

## License
This project is licensed under the [MIT License](LICENSE).

