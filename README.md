# Rubik's Cube Solver
This repository contains the code for a Rubik's Cube solving robot that can be run on a Raspberry Pi. It includes code for scanning the cube, finding a solution to the scramble, and commanding the servos in our robot to execute the solution. Most of the code--including our early cube-solving programs--are written in Python, though our main solver is written in C. Our solvers find near-optimal solutions using the CFOP method of solving, which is widely used in the speedcubing community. The C solver code is highly optimized to produce solutions in just a few seconds on the limited hardware of an old Raspberry Pi 1B+, with which we observe the average solve generation -> execute cycle to be around a minute with an average solution length of 30.22 moves in the half-turn metric.

Here's a video of it in action!
<video src=https://github.com/user-attachments/assets/ae13d80a-13bf-41fd-9e62-dea65046605f/ width=720 height=405>


## Supplies Needed
<img align="right" src="https://github.com/Laser-Space-Sharks/rubiks-cube-solver/blob/main/extras/RUBIKS.gif" width="175" height="250" alt="RUBIKS in the middle of solving a scramble."/>

- A raspberry pi
- 8 Servos capable of at least 180 degrees of rotation
  - We attempted both plastic and metal-geared servos and had more success with the latter
- Access to a 3D printer
- An LED and a button 
- breadboard, wires, cords, and the human spirit
  - you probably have these lying in a drawer somewhere
<br clear="right"/>

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

```sh
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
You do not need to understand how the solver represents cubes internally in order to build or use the robot. Everything from here and onward is for the mildly curious, or chronically bored. To help think about colorblind notation, the standard convention we have used is to associate U, R, F, L, B, and D with Yellow, Red, Blue, Orange, Green, and White, respectively. However, the robot may take the cube in any orientation and this will not affect the solve. 

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
The Cube18B is way of representing where each piece on the cube is, including piece orientation. There are 20 pieces on the cube, but because of parity and collisions, you only need to store 18 of them to uniquely represent any rubiks cube. We willfully forget 1 edge and 1 corner from the top layer of the cube. 1LLL allows us to avoid the consequences of doing such.

Every cubie we define as a position and an orientation of a piece on the cube, and there are 48 cubies. Originally, we represented every cubie as a tuple of which colors are on which faces, and every piece was represented as a solved cubie (the colors match the faces). So say the piece (F, R, D) was at cubie (F, R, D), that means the blue, red, and white piece is exactly where it would be on a solved cube. If however, that same (F, R, D) piece was instead at cubie (B, U, L), then the blue, red, and white facelets would be on the B, U, and L faces respectively. This completely encodes the position and orientation of the (F, R, D) piece on the cube. For performance and simplicity when coding, however, the cubies are instead defined in a 48-long enum s.t. you can also index an array with a cubie.

With this practice, since every cubie is less than 48, every cubie is a byte. The thusly named Cube18B is represented as an 18-long uint8_t array where every index is associated with a piece and every value is a cubie. 

The Solved Cube18B in our code:
```c
// Each of the 3 lines is a stage of CFOP too. Though OLL and PLL became 1LLL.
static const cube18B_s SOLVED_CUBE18B = {
    .cubies = {
        CUBIE_FD, CUBIE_RD, CUBIE_BD, CUBIE_LD, 
        CUBIE_FR, CUBIE_FRD, CUBIE_RB, CUBIE_RBD, CUBIE_BL, CUBIE_BLD, CUBIE_LF, CUBIE_LFD, 
        CUBIE_FU, CUBIE_RU, CUBIE_BU, CUBIE_URF, CUBIE_UBR, CUBIE_ULB
    }
};
```
This cube representation was made with the idea of wanting to find pieces rather than to see pieces, in order to recognize CFOP cases faster.
Cube18B brings further power though. To apply a move to Cube18B, you simply apply that move to all cubies individually.
You can take subsets of the set of cubies that is Cube18B, and you can apply moves on those and recognize cases with those. So Cube18B is completely modular.
In our code, Cube18B is ordered so that Cross, F2L, and 1LLL are neatly partitioned.

You can apply moves to cubies in any order. You could delay applying entire algorithms to certain parts of the cube in order to do less work.
But even better, that Cube18B hints at, is every move to a cubie is simply a vector to another possible cubie, of which there are 48.
So, you could have alongside algorithms, what they do to the 48 possible cubies. And then applying an algorithm to the cube is O(1) with respect to the number of moves in that algorithm.
And these tables of 48 cubies, interact with each other very nicely. These tables, may even represent cubes.

But alas, not Cube18B, nor its ideas, are used in the final product. The code is there, collecting dust.
The xcross subset of Cube18B, turns slightly faster than ShiftCube on some computers, but the feable attempts at hashing it made everything slower.
It did not get the optimization that it deserved. There was no time, and it broke too many heads.
Perhaps it shall be a next step.

## Cube Algorithms
The robot cannot do slice turns. So we use the face-turn metric. The robot can almost always do moves on opposite faces simultaneously whenever it can turn faces at all. 
But that would be a very different metric. The software would cry.

The solver is employing bidirectional search to find 1 xcross solution. It is then applying F2L and then it is applying 1LLL to solve the cube. 
Lastly, it is simplifying the concatenation of these algorithms.

To find algorithms, we originally looked online. However, algorithms online bring us problems.
Some of them use slice turns. Some of them use wide turns. Algorithms from different sources often have different move counts. Some algorithms from some sources, may not actually work.
The same is especially true for 'advanced' algorithms.

### F2L
We made a google sheet of all of the F2L cases on a per-pair basis.
The spreadsheet was used to collect algorithms from the internet. Ever improving algorithms with lesser and lesser move counts.
However, in order to reach optimality in a feasible amount of time, we computed our own F2L algorithms. 
The solver uses this F2L set. It does a search on all orderings to do the remaining 3 F2L pairs after xcross.
Since different algorithms that solve the same pair case, may have different effects on other unsolved pieces, the solver is also doing a search on all algorithms to use for each pair.

The spreadsheet now holds all of the FTM-optimal F2L algorithms for all of the 384 per-pair cases. 
We have a bot that can overwrite the F2L algorithms into the spreadsheet flawlessly should any damage be done.
God's Number for F2L pairs is 9. Experimentally, we find that there is almost always a way to use these F2L pair algorithms for an OLL and/or PLL skip.
Here is the spreadsheet:
https://docs.google.com/spreadsheets/d/1Agk1NxfVrzhe9LeRBR53sa6CxJYjCEaP81ebzhv8AHk/edit?usp=sharing

### 1LLL
The end of the solve is important. 
Not only does it take a large portion of the moves, but knowing the end of the solve means you don't have to apply those moves for any checking. 
That makes the software faster.

We went with 1LLL because it should have roughly half of the move count that OLL+PLL does, while also letting us know the rest of the solve sooner.
Originally, we used an OCR model to scan the 1LLL algorithms from a publicly available pdf. 
Of course, reformating them to have the types of moves that we tolerate, and fixing some minor errors like the OCR model reading 2 different algorithms in side-by-side cells as 1 jumbo algorithm.
All we would do is apply the inverse of those algorithms to associate them with cubes.
However, given that we want to know the end of the solve before we get there, it is crucial that we do not tolerate free spins of the top face at the end.
To expand 1LLL in full, 3916 algorithms became 62208. Using symmetry reduction and algorithm inversion, 62208 condenses into 4623.

However, we found that some 1LLL cases have 22 moves. That is larger than God's Number for the whole cube (20).
So we began a campaign to optimize 1LLL like we optimized F2L. Except we would just save the first algorithm we found, and we kept any algorithms that were already optimal.
Our 1LLL set is not in a nice human-readable format like F2L is. God's Number for 1LLL is 16.
Some statistics are here:
https://docs.google.com/spreadsheets/d/1Z_2Ahn18KJi1kAY5Obq5xqCyFUNY0ABMmeS0H8bgFF4/edit?usp=sharing

### Simplification
We do some basic simplification. 
"D D'" becomes "".
"D D2", becomes "D'".
And if we find a sequence of moves like "U D' U2", where 2 moves of the same face are separated by a move of the opposite face, then swapping moves of opposite faces gives us an opportunity to simplify.
So "U D' U2" becomes either "U' D'" or "D' U'".
