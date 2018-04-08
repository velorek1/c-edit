C-EDIT Project in C language
============================
C-EDIT - IN PROGRESS - An editor in the style of the MSDOS EDIT.

So far I have implemented:

* ANSI functions for positioning cursor in screen, changing output colour, hiding cursor and drawing box-like windows.
* A very simple Double Screen Buffer with a simple linked list in memory for greater display control. 
* Top-down menus using a double circular linked list.
* Kbhit and getch so that I can loop without blocking the program.
* Automatic display resizing.

Files:
=====
* c_cool.c -> code for the library that handles output with Ansi Functions and kbhit() in linux console (with its header file)
* list_choice.c -> code for the circular linked list responsible for handling menus 
* screen_buffer.c -> code for controlling display with a double screen buffer (with its header file)
* cedit.c -> Editor in C in the style of MSDOS EDIT (In progress).

As a screen buffer I have implemented a dynamic structure in memory that allows to save the current screen so that you can display new things on top and then go back to the previous (saved) screen. (simple linked list)

More info at: oldstuff286.blogspot.com

Makefile is included. Type "make" and "./cedit" to execute.

![Alt text](cedit1.png?raw=true "Demo")
![Alt text](cedit2.png?raw=true "Demo")

