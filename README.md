# 3D Gravity Simulation

A 3D Newtonian gravity simulator developed in C using the Raylib library.

![Simulation example](https://github.com/Gumaiagu/3D-Gravity/blob/main/media/example.gif)

## Compiling from Source Code

If you are using Linux, simply run `make`, you will need to have the libx11 package installed.

I haven’t tried running this program on a Windows machine or a Mac, so I don’t know how to compile it, but it shouldn’t be too difficult to compile in those cases.

## Running the program

After compilation, there will be a binary file named `simulation`; when you run it, a new borderless window will appear.

The window captures the mouse to control the camera. Using the A, W, S, D keys, you control its position. The spacebar moves it up and the Ctrl key moves it down. The mouse wheel zooms in or out from the point you are looking at. The Esc key closes the program.

To speed up the simulation, click J; to slow it down, click K. There is no hard-coded maximum speed, but the minimum speed is 0, which means time stands still (I couldn’t get the simulation to run in reverse).

You can edit the index.csv file to change the program’s initial state.

The index.csv should be written like this (Tabs and Spaces are ignored):

```csv
Posx,Posy,Posz,Velx,Vely,Velz,mass,radius,r,g,b
0,   0,   0,   0,   0,   0,   3e16,10,   255, 0, 0
```

> Be careful with the index.csv file; if it is misconfigured, it can cause a segmentation fault or make an object disappear. Fortunately, none of these issues are permanent.

## Use of AI

In this project, I did not use AI to write code, it was used only to research the functions and types of C and Raylib.
