# Rubik's Cube Solver

This repository contains the code for a Rubik's cube-solving robot that can be run on a raspberry pi. It includes code for scanning the cube, finding a solution to the scramble, and commanding the servos in our robot to execute the solution. Primarily written in python and C, our robot finds near optimal solutions in a short amount of time that can be quickly executed by the machine. On our raspberry pi one, this means on average each solve execute cycle takes around a minute with an average of 30.22 moves. 

## Wait! I want a rubiks cube solving robot
Well it looks like you've come to the right place!

### Supplies Needed
- A raspberry pi
- 8 Servos capable of at least 180 degrees of rotation
  - We attempted with both plastic and metal geared servos and had more success with the latter. However beware of current issues
- Access to a 3D printer
- An LED and a button 
- breadboard, wires, cords, and the human spirit
  - you probably have these laying in a drawer somewhere
 
### Dependencies 

### Running the software
**Clone github repo to pi**:
Run this command 

```git clone https://github.com/Laser-Space-Sharks/rubiks-cube-solver```

**Compiling solver.C for your device**:

**Run the Program**:
Simply run ``python RUBIKS.py`` in the ``rubiks-cube-solver`` folder to get the robot to initalize and get ready for a solve. 

NOTE: On a rasbperry pi one this takes a hot second. The LED will light up once it is ready to go! once the light is on, put the cube in, press the button, and watch as the cube is solved!

If you would like the pi to run RUBIKS on start up add the following to the .bashrc file on the pi. 


### How to Build and Use!
The first thing that you will have to do to get started on this endevour is to obtain a raspberry pi or an equivalent microcomputer. It doesn't have to be powerful or new for our code to run well- in fact our group used a decade old raspberry pi found in a dusty box for this project- so anything you have laying around that can run linux will do. 

