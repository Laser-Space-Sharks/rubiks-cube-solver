# Rubik's Cube Solver

This repository contains the code for a Rubik's cube-solving robot that can be run on a raspberry pi. It includes code for scanning the cube, finding a solution to the scramble, and commanding the servos in our robot to execute the solution. Primarily written in python with a solver written in C, our robot finds near optimal solutions in a short amount of time that can be quickly executed by the machine. On our raspberry pi1, this means on average each solve execute cycle takes around a minute with an average of 30.22 moves. 

## Supplies Needed
- A raspberry pi
- 8 Servos capable of at least 180 degrees of rotation
  - We attempted with both plastic and metal geared servos and had more success with the latter
- Access to a 3D printer
- An LED and a button 
- breadboard, wires, cords, and the human spirit
  - you probably have these laying in a drawer somewhere
 
## Dependencies
Python Packages:
  - RPi.GPIO
  - numpy
  - openCV
  - piCamera 2 (or libcamera)
  - gpiozero

Other software needed on Pi:
  - pigpio
  - libcamera (or PiCamera 2)

## Running the software
Run this command to clone this repo to the pi:

```git clone https://github.com/Laser-Space-Sharks/rubiks-cube-solver```

### Compiling solver.C for your device:
more info to come

### Run the Program:
Simply run ``python RUBIKS.py`` in the ``rubiks-cube-solver`` folder to get the robot to initialize and get ready for a solve. 

NOTE: On a raspberry pi1 this takes a hot second. The LED will light up once it is ready to go! Once the light is on, put the cube in, press the button, and watch as the cube is solved!

If you would like the pi to run RUBIKS on start up add the following to the .bashrc file on the pi. 

```
if [[ "$(tty)" == "tty1" ]] then
    /[YOUR PATH TO REPO]/run.sh
fi
```

**Running the solver by itself**

The solver is the part of our program that takes in a cube scramble and generates an algorithm that will solve the cube. You can use this program in the terminal without running the robot by running the command ``./solver [OPTION]... INPUT...`` where input is the scrambled cube written in a "Shift Cube" format, or the moves required to scramble the cube. To learn more about how to use the solver run ``./solver --help``

## How to Build and Use!
The first thing that you will have to do to get started on this endeavor is to obtain a raspberry pi or an equivalent microcomputer. It doesn't have to be powerful or new for our code to run well- in fact our group used a decade old raspberry pi found in a dusty box for this project- so anything you have laying around that can run Linux will do. Next download the 3D print file models for the container that the pi and breadboard will sit in, the mechanism that holds the servos and the cube, and the servo claws.

Once all of the software is installed (see above for details) the wiring needs to be completed. More info to come

## Cube Representations
You do not need to understand how the solver represents cubes internally in order to build or use the robot. This section is for the mildly curious, or chronically bored.

### ShiftCube
more info to come

### Cube18B
more info to come

