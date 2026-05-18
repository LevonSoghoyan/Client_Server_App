# C Client-Server Remote Shell Application

A robust, low-level networking application written in C. This project implements a client-server architecture using Berkeley Sockets, allowing for remote system management and command execution via a custom shell interface.

##  Compilation and Build

The project uses a Makefile for easy compilation.

Build the binaries:
    
    $ make server

    $ make client

Remove build artifacts:
    
    $ make clean

---

##  Server Operations

The server handles multiple connections and monitors system activity.

USAGE:

Run the server binary: ./server

COMMANDS:
  
  up                Start the server and begin listening for connections.
  
  down              Stop the server and release the port.
  
  client            Display a list of all currently connected clients.
  
  status            Show current server status (Online/Offline) and stats.
  
  ps                List active processes on the server-side system.
  
  history           Show a log of all shell commands received and executed.

---

##  Client Operations

The client connects to the server to perform remote administrative tasks.

USAGE:

Run the client binary: ./client

COMMANDS:

  connect <IP>      Establish a TCP connection to the server at the specified IP.

  shell <command>   Execute a system command in the server's terminal.

  status            Check if the client is currently connected or disconnected.

  disconnect        Terminate the current session safely.
