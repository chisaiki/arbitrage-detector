# Notes

## Library Choice
Boost.Beast and Boost.Asio

**Why?**


## WebSocket Program Structure (Conceptual)

### Lifecycle Steps:

**1. Initialization Phase**
- Set up the event loop (the "engine" that runs everything)
- Configure SSL/TLS context if using secure connections (wss://)
- Prepare any authentication credentials if needed

**2. Connection Establishment**
- Specify the target URL (e.g., wss://stream.binance.com:9443/ws)
- Initiate the handshake (HTTP upgrade to WebSocket protocol)
- Wait for connection success or failure

**3. Event Registration (Define Callbacks)**
- **onOpen**: What happens when connection succeeds
- **onMessage**: What happens when data arrives from server
- **onClose**: What happens when connection terminates
- **onError**: What happens if something goes wrong

**4. Send Subscription Messages**
- Once connected, tell the exchange what data you want
- Usually JSON messages like: "subscribe to BTC/USD ticker"

**5. Message Processing Loop**
- Event loop runs continuously
- Automatically calls your callbacks when events occur
- Your onMessage handler processes incoming price data
- Push data to your lock-free queues for your detector threads

**6. Graceful Shutdown**
- Send unsubscribe messages (optional but polite)
- Close the websocket connection
- Clean up resources

### Key Concepts:

**Event-Driven Model**
- You don't poll for messages in a loop
- The library calls YOUR functions when things happen
- Think: "register callbacks, then let it run"

**Threading Model**
- The websocket runs in its own event loop
- You'll need to safely hand off data to your arbitrage detector threads
- This is where your lock-free queues come in

**Connection Management**
- Handle reconnection if connection drops
- Implement heartbeat/ping-pong to keep connection alive
- Detect stale connections

### For Multiple Exchanges:
- Run multiple websocket connections (probably in separate threads)
- Each has its own event loop and callbacks
- All feed into your central price aggregation system

# ERRORS

## Error: VS Code IntelliSense Cannot Find Boost Headers in WSL

**Symptom:** The C++ code compiles perfectly using `cmake` and `make` in the WSL terminal, but VS Code displays red squiggles under `#include <boost/asio.hpp>` and throws a `fatal error: No such file or directory`. 

**Root Cause:**
A disconnect between the compiler environment and the editor. The compiler lives inside Linux (WSL) and knows the headers are at `/usr/include/boost`. However, the VS Code UI is running on Windows and is trying to find the Boost library on the `C:\` drive. VS Code needs to be explicitly told to look through the Linux lens and ask CMake for the correct paths.

## SOLUTION: 

**The Concept:**
To develop in a Linux environment while using the Windows version of VS Code, you must install a "bridge." This bridge allows the Windows UI to talk to a "VS Code Server" running inside your WSL distribution. Without this, VS Code will search your Windows `C:\` drive for compilers and headers (like Boost), which do not exist there.

### 1. Installation
1. Open VS Code on Windows.
2. Open the **Extensions** view (`Ctrl` + `Shift` + `X`).
3. Search for **WSL** (published by Microsoft).
4. Click **Install**.

### 2. The "Remote" Workflow
To ensure your project opens with the correct Linux context:
1. Open your **WSL Terminal** (e.g., Ubuntu).
2. Navigate to your project folder: `cd ~/path/to/project`.
3. Launch VS Code from the terminal: `code .`

### 3. Verification (The Remote Badge)
Always check the **Status Bar** at the very bottom-left of the VS Code window.
* **Success:** You see a green or blue box labeled **WSL: [Distro Name]** (e.g., `WSL: Ubuntu`).
* **Failure:** The box is missing or says "Local." If this happens, your IntelliSense will likely show red squiggles for Linux-specific libraries.

### 4. Why this is necessary for Systems Programming
* **Headers:** Boost and other libraries installed via `apt` live in `/usr/include`, which Windows cannot natively see.
* **Compilers:** The extension tells VS Code to use the Linux `g++` and `gdb` for compiling and debugging, rather than searching for Windows-based compilers like MinGW.
* **Performance:** Running the compiler and linker natively inside the Linux file system is significantly faster than trying to compile across the network drive boundary.

So I basically just created a WSL based instance of VSCode. Its only within the scope of this project according to Gemini, but I'm not so sure.