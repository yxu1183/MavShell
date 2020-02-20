# Mav Shell
### Operating System/Assignment-1: Created by Dr. Trevor Bakker

## Description
A shell program - Mav shell (msh), similar to bourne shell (bash), c-shell (csh), or korn shell (ksh) with basic functionalities. It accepts the command, navigates through the file system, and executes those commands. 

## Functionality
* Shell supports any command in /bin,/usr/bin/,/usr/local/bin/, and the current working directory. Parameters may also be combined. Example: ps -aef or ps -a -e -f.
* Supports the showpids command to list the PIDS of the last 15 process spwaned by the shell.
* Supports the history command to list the last 15 commands entered by the user. 
* Executes any command from history. Typing !n, where n is a number between 1 and 15 will result in re-running the nth command in shell.

## Files Included
* [msh.c](https://github.com/yxu1183/CSE-3320-MavShell/blob/master/msh.c) - Main source code which executes the main program (Mav Shell).
* [fork.c](https://github.com/yxu1183/CSE-3320-MavShell/blob/master/fork.c)- Code that demonstrates the basic concept used in the main source code.
* [zombie.c](https://github.com/yxu1183/CSE-3320-MavShell/blob/master/zombie.c) - Code that demonstrates the possible error that might occur while executing fork function.

## Things I Learned
* Shell commands and functionalities in UNIX systems.
* Use of fork to handle multiple processes at a time.

## Acknowledgements
[Dr. Trevor Bakker](https://github.com/trevorbakker-uta) for providing small stub [programs](https://github.com/CSE3320/Shell-Assignment) to get started and his guidance throughout the assignment. 


