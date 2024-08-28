C-EDIT Project in C language (NO NCURSES)
=========================================
C-EDIT for linux - ALPHA - A linux text editor in the style of the MSDOS EDIT - WITHOUT USING NCURSES
Works well with Termux! :)
* WARNING: DO NOT EDIT YOUR PRECIOUS FILES WITH THIS EDITOR!!

**If you want to support this project: https://tippin.me/@Velorek
Contact me at: velorek1@gmail.com**

**NEW: AUGUST 2024 (merging with ceditbuf)**
- Dynamic buffer added (vector of lines) 

History:
- 2023 -> Previous demo version kept in https://github.com/velorek1/C-editold
- 2022 -> I have rewritten the screenbuffer and fixed polling issues (lynx: https://github.com/velorek1/lynx). 
Ideally, a keyboard library abstraction would be nice to expand the editor's possibilities.**
- 2018 -> Start of the project; basic demo concepts developed (https://oldstuff286.blogspot.com/2018/03/c-edit-in-progress.html)

TO INSTALL:  

    * Download or clone repository.
    * Type "cd src", "make" and "./cedit" to execute.
    
So far I have implemented:

* A very simple Double Screen Buffer with a simple linked list in memory for greater display control. 
* Rudimentary Text User Interface with Windows, Textbox, Keyboard handling, Color schemes, etc.
* Automatic display resizing.
* Open file dialog with a Listbox and SCROLL capabilities to navigate and find the file to open.
* Rudimentary Edit buffer with vertical and horizontal scroll (vector of lines). 
* A millisecond timer for animations.
* Timers and kbhit with no polling for optimate CPU usage.
* Greater modularity than Demo version

Files in /src/:
===============
* rterm.c -> code for the library that handles output with Ansi Functions and kbhit() in linux console (with its header file)
* listbox.c -> code for the circular linked list responsible for handling menus 
* scbuf.c -> code for controlling display with a double screen buffer (with its header file)
* keyb.c -> module to control keyboard input.
* opfile.c -> module that list files with scroll capabilities to select the file to open.
* ui.c -> module with user interface widgets: alert windows, confirmation windows, textbox, etc.
* fileb.c -> module for single file operations.
* tm.c -> module for a millisecond timer for animations (C11)
* main.c -> Editor in C in the style of MSDOS EDIT (In progress).
* global.c -> Global variables and main animation
* menu.c -> Drop-down menu
* editor.c -> Code for the editor section
* edbuf.c -> Buffer - vector of lines


As a screen buffer I have implemented a dynamic structure in memory that allows to save the current screen so that you can display new things on top and then go back to the previous (saved) screen. (simple linked list)

TO-DO:
* Clean code
* Add more functionality to the editor (home/end/ page up keys , del key etc)
* Code scroll bars
* Finish menus (help, about, file info, etc)


![Alt text](cedit3.jpg?raw=true "Demo")
![Alt text](cedit4.jpg?raw=true "Demo")
