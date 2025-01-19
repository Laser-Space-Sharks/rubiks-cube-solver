# Rubik's Cube Solver

This repository contains the code for a Rubik's cube-solving robot that can be run on a Raspberry Pi. It includes code for scanning the cube, finding a solution to the scramble, and commanding the servos in our robot to execute the solution. Primarily written in Python with a solver written in C, our robot finds near-optimal solutions in a short amount of time that can be quickly executed by the machine. On our raspberry pi1, this means on average each solve execute cycle takes around a minute with an average of 30.22 moves. 

## Supplies Needed
- A raspberry pi
- 8 Servos capable of at least 180 degrees of rotation
  - We attempted both plastic and metal-geared servos and had more success with the latter
- Access to a 3D printer
- An LED and a button 
- breadboard, wires, cords, and the human spirit
  - you probably have these lying in a drawer somewhere
 
## Dependencies
Python Packages:
  - RPi.GPIO
  - NumPy
  - OpenCV
  - piCamera 2 (or libcamera)
  - gpiozero

Other software needed on Pi:
  - pigpio
  - libcamera (or PiCamera 2)

## Running the software
Run this command to clone this repo to the pi:

```git clone https://github.com/Laser-Space-Sharks/rubiks-cube-solver```

### Run the Program:
Simply run ``python RUBIKS.py`` in the ``rubiks-cube-solver`` folder to get the robot to initialize and get ready for a solve. 

NOTE: On a raspberry pi1 this takes a hot second. The LED will light up once it is ready to go! Once the light is on, put the cube in, press the button, and watch as the cube is solved!

If you would like the pi to run RUBIKS on startup add the following to the .bashrc file on the pi. 

```
if [[ "$(tty)" == "tty1" ]] then
    /[YOUR PATH TO REPO]/run.sh
fi
```

**Running the solver by itself**

The solver is the part of our program that takes in a cube scramble and generates an algorithm that will solve the cube. You can use this program in the terminal without running the robot by running the command ``./solver [OPTION]... INPUT...`` where input is the scrambled cube written in a "Shift Cube" format, or the moves required to scramble the cube. To learn more about how to use the solver run ``./solver --help``

## How to Build and Use!
The first thing that you will have to do to get started on this endeavor is to obtain a Raspberry Pi or an equivalent microcomputer. It doesn't have to be powerful or new for our code to run well- in fact, our group used a decade-old Raspberry Pi found in a dusty box for this project- so anything you have lying around that can run Linux will do. Next download the 3D print file models for the container that the pi and breadboard will sit in, the mechanism that holds the servos and the cube, and the servo claws.

Once all of the software is installed (see above for details) the wiring needs to be completed. More info to come

Tips and Tricks for getting everything set up:
  - With so many servos, something is bound to be unhappy at some point. Make sure to not overheat the servos and double-check that everything has enough amps to run. 
  - Additionally, ``ServoController.py`` can be run to test which angles hold the cube the best. It usually differs for every servo, so testing is best. 
  - Once everything was all wired up, we screwed our 3D prints to a slab of wood. This not only made the robot more easily portable, but it drastically decreased the amount of time that we had to spend lining the camera up to make sure it could see the cube effectively.
  - On lining up the camera, libcamera-still can be used to see what the robot will see while it is scanning. Use this to test if the camera can see the cube, and make sure the camera is not titled, which will cause some pieces to be larger than others. 

## Cube Representations
You do not need to understand how the solver represents cubes internally in order to build or use the robot. This section is for the mildly curious, or chronically bored. For convenience when thinking about colors on the cube, we associate U, R, F, L, B, and D with Yellow, Red, Blue, Orange, Green, and White respectively. 

### ShiftCube
The ShiftCube is a way of representing the faces of the cube with 6 32-bit unsigned integers. Each piece is represented with a nibble that corresponds to the face that that piece should belong to when the cube is solved. The ShiftCube is colorblind, so we represent the faces by orientation. In this notation Up/U = 0, Right/R = 1, Front/F = 2, Left/L = 3, Back/B = 4, Down/D = 5. The order the pieces are stored in is depicted below. Note the center is redundant information because we know the center based on what face it is on. 

```
this represents the face of the cube, and here the numbers 
represent the order of the pieces as they are stored in the ShiftCube.
[[0] [1] [2]]
[[7] [x] [3]]
[[6] [5] [4]]
```

### Cube18B
The Cube18B is way of representating where each piece on the cube is, including piece orientation. There are 20 pieces on the cube, but because of parity and collisions, you only need to store 18 of them to uniquely represent any rubiks cube.

Every cubie we define as a position and an orientation of a piece on the cube, and there are 48 cubies. Originally, we represented every cubie as a tuple of which colors are on which faces, and every piece was represented as a solved cubie (the colors match the faces). So say the piece (F, R, D) was at cubie (F, R, D), that means the blue, red, and white piece is exactly where it would be on a solved cube. If however, that same (F, R, D) piece was instead at cubie (B, L, U), then the blue facelet would be on the B face, the red facelet would be on the L face, and the white facelet would be on U face. This completely encodes the position and orientation of the (F, R, D) piece on the cube. For performance and simplicity when coding however, the cubies are instead defined in a 48-long enum s.t. you can also index an array with a cubie.

With this practice, since every cubie is less than 48, every cubie is a byte. The thusly named Cube18B is thus represented as an 18-long uint8_t array where every index is associated with a piece and every value is a cubie. 

The Solved Cube18B in our code:
```
// Each of the 3 lines is a stage of CFOP too. Though OLL and PLL became 1LLL.
static const cube18B_s SOLVED_CUBE18B = {
    .cubies = {
        CUBIE_FD, CUBIE_RD, CUBIE_BD, CUBIE_LD, 
        CUBIE_FR, CUBIE_FRD, CUBIE_RB, CUBIE_RBD, CUBIE_BL, CUBIE_BLD, CUBIE_LF, CUBIE_LFD, 
        CUBIE_FU, CUBIE_RU, CUBIE_BU, CUBIE_URF, CUBIE_UBR, CUBIE_ULB
    }
};
```
Breaking the pieces up also allows you to apply moves and cube algorithms to pieces individually and out-of-order. When recognizing cases in CFOP, it is much more useful to know where piece x is than to know what piece is at x.