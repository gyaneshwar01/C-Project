# C Web Server

This is a simple web server implemented in C. It can handle basic HTTP requests and serve static HTML content to clients.

## Features

- Listens on a specified port for incoming connections.
- Handles basic HTTP GET requests.
- Serves HTML content.

## Prerequisites

- Windows OS (The code is tailored for Windows, but can be adapted for other platforms)
- C complier and standard library

## How to Run

1. Clone the repository to your local machine:
2. Compile the code using the command
   ```bash
     gcc webserver.c -o output -lws2_32
   ```
3. Run the executable i.e. output.exe
4. Open your browser and goto http://localhost:8080
