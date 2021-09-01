C-EDIT Project in C language (NO NCURSES)
=========================================
C-EDIT for linux - IN PROGRESS - A linux text editor in the style of the MSDOS EDIT - WITHOUT USING NCURSES
* WARNING: DO NOT EDIT YOUR PRECIOUS FILES WITH THIS EDITOR!!

I'm working on adding the Dynamic buffer(vector of lines) -> tests in https://github.com/velorek1/ceditbuf

If you want to support this project:
https://tippin.me/@Velorek

TO INSTALL:  

    * Download or clone repository.
    * Type "cd src", "make" and "./cedit" to execute.
    
So far I have implemented:

* A very simple Double Screen Buffer with a simple linked list in memory for greater display control. 
* Rudimentary Text User Interface with Windows, Textbox, Keyboard handling, Color schemes, etc.
* Automatic display resizing.
* Open file dialog with a Listbox and SCROLL capabilities to navigate and find the file to open.
* Rudimentary Edit buffer with vertical scroll. 
* A millisecond timer for animations.

Files in /src/:
===============
* rterm.c -> code for the library that handles output with Ansi Functions and kbhit() in linux console (with its header file)
* list.c -> code for the circular linked list responsible for handling menus 
* scbuf.c -> code for controlling display with a double screen buffer (with its header file)
* keyb.c -> module to control keyboard input.
* opfile.c -> module that list files with scroll capabilities to select the file to open.
* uintf.c -> module with user interface widgets: alert windows, confirmation windows, textbox, etc.
* fileb.c -> module for single file operations.
* tm.c -> module for a millisecond timer for animations (C11)
* main.c -> Editor in C in the style of MSDOS EDIT (In progress).


As a screen buffer I have implemented a dynamic structure in memory that allows to save the current screen so that you can display new things on top and then go back to the previous (saved) screen. (simple linked list)

![Alt text](cedit3.jpg?raw=true "Demo")
![Alt text](cedit4.jpg?raw=true "Demo")
