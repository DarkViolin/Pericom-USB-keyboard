# Pericom-USB-keyboard
Converter between the Pericom 7800 teriminal keyboard and USB


This project started just before the end of 2017.

My goal is to create a converter box for using a 1983 Pericom terminal keyboard (orginally used with the Pericom 7800 graphics terminal) as a USB keyboard. The keyboard and terminal looks very similar to the VT100 keyboard and SET-UP software and so on, but is not electrically similar.

This keyboard received continuous updates from the terminal for updating the status lights and for sounding the beeper.
The keyboard doesn't send any information until a key is pressed, or released.


Keyboard is feeded with 5 volt, drawing around 450mA.
Serial outputs / input is upside down (inverted TTL).


Signal looks like this:
--------------------------------------------------------------------
1200 baud both ways. 1 startbit, 8 databits, no parity, 1 stopbit.

From terminal
--------------------------
-= STARTBIT =-
Online / Local
Setup A-B-C
L1 Toggle I/O
L2 TX Speed
L3 RX Speed
L4 80/132
L5 RESET
Beeper signal
-= STOPBIT =-

From keyboard
--------------------------
-= STARTBIT =-
SCANCODE
SCANCODE
SCANCODE
SCANCODE
SCANCODE
SCANCODE
SCANCODE
Key press/Key release
-= STOPBIT =-




