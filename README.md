# OS-Project

## Overview

This project implements a client-server architecture for a key-value store system. The server handles requests from clients, processes operations, and manages data storage. The client provides an interface for interacting with the server. The project is modular, with shared components for common functionality.

## Features

- **Client-Server Architecture**: Implements a distributed system with separate client and server components.
- **Key-Value Store**: The server manages a key-value store with operations for data manipulation.
- **Modular Design**: Shared components for common functionality, such as I/O and protocol handling.
- **Code Formatting**: Includes a `format` target in the Makefile for consistent code style using `clang-format`.

## How to Build

The project uses a `Makefile` for building the client and server components. To build the project, run:
`make`

This will generate the following executables:

- `src/server/kvs`: The server executable.
- `src/client/client`: The client executable.

## How to Run
Running the Server
To start the server, run:
`./src/server/kvs`

Running the Client
To start the client, run:
`./src/client/client`

Makefile Targets
- `all`: Builds both the client and server.
- `clean`: Removes all compiled files and executables.
`make clean`
- `format`: Formats all source code files using `clang-format`.
`make format`

## Dependencies
- Compiler: GCC with support for C17.
- Libraries: POSIX-compliant system for threading (`-pthread`).
- Tools: `clang-format` for code formatting.

## Example Usage
1. Start the server:
`./src/server/kvs`
2. Start the client:
`./src/client/client`
3. Interact with the server through the client interface.

## Author
Developed by Guilherme Monteiro. For more information, visit [my GitHub profile](https://github.com/Monteir016).
