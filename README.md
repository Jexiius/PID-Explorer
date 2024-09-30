# Process Info Reader

This C++ program retrieves information about a process by its PID from the `/proc` directory on Linux systems. The program collects details such as the process's username, name, command line, RSS (Resident Set Size), and PSS (Proportional Set Size) memory usage.

## Features
- **Retrieve Process Information by PID**: Get details about a running process by providing its PID.
- **Memory Information**: Retrieve both RSS and PSS values for memory usage.
- **User Information**: Identify the user who initiated the process.
- **Command Line and Process Name**: Extract the command used to launch the process and the process's name.

## Requirements
- Linux-based system with access to the `/proc` directory.
- C++11 or newer compiler.

## Compilation
To compile the program, use the following command:

`make procInfo`

Run

`procInfo -h`