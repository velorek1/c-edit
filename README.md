# C-projects
Projects in C language
======================

*A Very Simple Double Screen Buffer with top-down menus using Ansi-functions.*

* c_cool.c -> code for the library that handles output with Ansi Functions in linux console (with its header file)
* list_choice.c -> code for the circular linked list responsible for handling menus and screen buffer (with its header file)
* test1.c -> Test program that displays top-down menus.

As a screen buffer I have implemented a dynamic structure in memory that allows to save the current screen so that you can display new things on top and then go back to the previous (saved) screen.
More info at: oldstuff286.blogspot.com

Makefile is included. Type "make" and "./test1" to execute.

![Alt text](output.gif?raw=true "Demo")
