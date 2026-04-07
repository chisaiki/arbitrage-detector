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
