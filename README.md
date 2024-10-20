# ResilientNet for STM32F411RE

## Overview
This project implements a simplified version of the ResilientNet architecture on the STM32F411RE microcontroller using bare-metal programming. The code provides a basic framework for initializing hardware and executing a forward pass of a neural network.

## Features
- **Bare-Metal Implementation**: No operating system is used, making it suitable for resource-constrained environments.
- **GPIO Initialization**: Configures GPIO for LED blinking.
- **Forward Pass**: A basic implementation of a forward pass through a fully connected layer.

## Hardware Requirements
- **STM32F411RE** development board
- **LED** connected to pin PA5 (onboard LED on most boards)

## Usage

1. Connect your STM32F411RE board to your computer.
2. Flash the binary to the board.
3. Upon powering the board, the onboard LED connected to PA5 will blink at a predefined rate, indicating that the forward pass function is being executed.

## Customization

- You can modify the weights and biases arrays in `main.c` to experiment with different neural network configurations.
- Add activation functions and additional layers to expand the functionality of the ResilientNet implementation.

## Troubleshooting

- Ensure that the correct GPIO pin is configured for the LED.
- Verify that the toolchain is properly installed and added to your system's PATH.
- If the LED does not blink, check your connections and ensure that the code has been flashed correctly.

## Acknowledgments

This project is inspired by neural network architectures and embedded systems programming.

Feel free to reach out if you have any questions or feedback.
