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

### Solution: The CMake-to-IntelliSense Handoff

**Step 1: Open VS Code from inside WSL**
Do not open VS Code from the Windows Start Menu and navigate to the network drive.
1. Open your WSL terminal.
2. Navigate to the project directory.
3. Run the command: `code .`
*(Verify this worked by checking the bottom-left corner of VS Code for a blue/green badge that says **WSL: Ubuntu**).*

**Step 2: Install Required Extensions (In WSL)**
Ensure the following Microsoft extensions are installed and explicitly enabled *within the WSL environment*, not just locally on Windows:
* **C/C++** * **CMake Tools** **Step 3: Change the Configuration Provider**
Tell the C/C++ extension to stop guessing file paths and get the exact locations directly from your `CMakeLists.txt`.
1. Open the VS Code Command Palette (`Ctrl` + `Shift` + `P`).
2. Type and select: **`C/C++: Change Configuration Provider...`**
3. Select **`CMake Tools`** from the dropdown menu.
4. *(Optional)* Open the Command Palette again and run **`CMake: Configure`** to force an immediate refresh. 

**Result:** The red squiggles will disappear, and hovering over Boost objects will correctly display the official documentation.