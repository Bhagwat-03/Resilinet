# Resilinet


## Overview

This project implements a simple node management system using the STM32F411RE microcontroller. It includes functionalities for adding and removing nodes based on activity, while entering low-power mode when no nodes are active. The system also toggles an onboard LED to indicate activity.

## Features

- Add and remove nodes dynamically based on activity.
- Enter low-power mode when no nodes are active.
- Onboard LED indicates activity by blinking.
- Button interrupt simulates the addition of new nodes.

## Usage

1. Connect your STM32F411RE board to your computer.
2. Flash the binary to the board.
3. Upon powering the board, press the button connected to PC13 to add a new node. The onboard LED connected to PA5 will blink, indicating activity.

## Customization

- You can modify the `MAX_NODES` definition to change the maximum number of nodes.
- Adjust the `TIMEOUT` value to set the inactive node timeout period.
- Modify the LED pin configuration in the `GPIO_Init` function if you are using a different pin.

## Troubleshooting

- Ensure that the correct GPIO pin is configured for the LED and button.
- Verify that the toolchain is properly installed and added to your system's PATH.
- If the LED does not blink, check your connections and ensure that the code has been flashed correctly.

## Acknowledgments

This project demonstrates basic node management techniques and low-power operation using STM32 microcontrollers.

Feel free to reach out if you have any questions or feedback.
