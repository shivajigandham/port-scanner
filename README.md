# Advanced C++ Port Scanner

A high-performance, multi-threaded port scanner built with C++ and Winsock2.

## Features
- **Fast Multi-threading**: Scan thousands of ports in seconds using a configurable thread pool.
- **Service Detection**: Automatically identifies common services like HTTP, SSH, FTP, etc.
- **Banner Grabbing**: Retrieves identification strings from open ports.
- **Interactive UI**: Clean, colorized terminal interface with a progress bar.
- **Customizable**: Choose target IP, port ranges, and thread counts.

## Prerequisites
To compile this project on Windows, you need a C++ compiler. Recommended options:
1. **MinGW-w64**: Provides `g++`. (Download from [mingw-w64.org](https://www.mingw-w64.org/))
2. **Visual Studio**: Includes `cl.exe`. (Download Visual Studio Community)

## How to Compile

### Using MinGW (g++)
Open your terminal and run:
```bash
g++ port_scanner.cpp -o port_scanner.exe -lws2_32
```

### Using Visual Studio (Developer Command Prompt)
```cmd
cl /EHsc port_scanner.cpp /link ws2_32.lib
```

## How to Use
1. Run the compiled `port_scanner.exe`.
2. Enter the target IP (e.g., `127.0.0.1` for local scan).
3. Enter the start and end port numbers (e.g., `1` to `1000`).
4. Enter the number of threads (e.g., `100` for fast scanning).
5. Watch the progress bar and wait for results!
