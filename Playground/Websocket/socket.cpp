/*Goal: Build a websocket connection with an open source -- 
Connect to a server, receive data, print it.*/

/*Plan:
Includes and setup (10 lines)
WebSocket callbacks (30-50 lines)
Main function (20 lines)
Maybe a simple JSON parser helper (30 lines)*/

/*In Order to Implement a Websocket:

1. Handshake: where the server and client establish connection. 
Client sends the server an HTTP request (IDK THE FORMAT)
and within that request, it asks for a potential upgrade to websocket
Connection: Upgrade
Upgrade: Websocket
Sec-WebSocket-Key: base 64-encoded random nonce (used for verification with server connection)
101 Switching Protocol 

2. Framing and Masking: 
HTTP Text based to Binary Based framing
FIN Bit: indicates end of the frame bit
Opcode: the type of data sent 
Masking: All sent to the server must be masked with a 32-bit key

3. Data Transfer (Full-Duplex)
        - Asynchronous Polling: Keep an event loop running to listen 
        for any incoming bytes 
        - Buffer Managment: Store partial frames until the FIN bit is found

4. Control Frame
Keeps the connection alive, because you dont know when the server will send data
and you need to periodically check if the connection is still valid

Ping (0x9) waits for a pong (0xA)

5. Close The Handshake
- Send a Closing Frame (0x8)
- Server / client has to respond with a close frame 
- Terminate connection
*/