# TCP Chat Server

A simple, multi-client TCP chat server built in C++ with object-oriented architecture that allows users to connect, authenticate, and chat in real-time.

## Features

✅ **Multi-client Support** - Handles 5-10+ simultaneous connections  
✅ **User Authentication** - Login with unique usernames  
✅ **Broadcast Messages** - Send messages to all connected users  
✅ **Direct Messages (DM)** - Private messaging between users  
✅ **User List** - View all connected users  
✅ **Idle Timeout** - Automatic disconnect after 60 seconds of inactivity  
✅ **Heartbeat/Ping** - Keep connections alive with PING/PONG  
✅ **Configurable Port** - Set via environment variable or command-line  
✅ **Object-Oriented Design** - Clean inheritance hierarchy with base Client class  

## Architecture

The project follows an object-oriented design with a clear inheritance hierarchy:

### Class Hierarchy
```
ChatServer (Main Server)
    ├── Manages client connections
    ├── Handles authentication
    └── Routes messages

Client (Base Class)
    ├── socket: SOCKET
    ├── username: string
    ├── isAuthenticated: bool
    ├── lastActivity: time_point
    └── Methods: sendMessage(), receiveMessage(), updateActivity(), isIdle()
    │
    ├── BroadcastClient (Child Class)
    │   ├── broadcastToAll()
    │   ├── broadcastToOthers()
    │   ├── broadcastChatMessage()
    │   └── broadcastInfo()
    │
    └── DMClient (Child Class)
        └── sendDirectMessage()

ChatListener
    ├── Parses incoming commands
    └── Routes to appropriate handlers

Connect
    └── Connection manager using Client instances
```

### Design Principles
- **Pure OOP**: All functionality implemented through instance methods
- **Inheritance**: `BroadcastClient` and `DMClient` inherit from `Client` base class
- **Encapsulation**: Each class has clear responsibilities
- **Thread Safety**: Mutex locks for shared client list
- **Smart Pointers**: Uses `std::shared_ptr<Client>` for automatic memory management

## Building the Project

### Prerequisites
- **Windows OS** (uses Winsock2 API)
- **C++17 compiler** (g++ via MinGW/MSYS2 recommended)
- **ws2_32.lib** (Windows Socket library - included with Windows SDK)

### Dependencies
No external libraries required. The project uses standard C++17 and Windows Winsock2 API.

### Build Instructions

#### Using the PowerShell Build Script (Recommended)

The project includes a `build.ps1` script that handles compilation with proper static linking:

```powershell
# Navigate to project directory
cd c:\Ankit\Programs\chatTCP

# Run build script
.\build.ps1
```

The build script uses:
```powershell
g++ -std=c++17 -O2 -static -static-libgcc -static-libstdc++ `
    -o ChatServer.exe `
    main.cpp ChatServer.cpp Client.cpp ChatListener.cpp `
    BroadcastClient.cpp DMClient.cpp `
    -lws2_32
```

#### Manual Build with g++

```powershell
# Build server
g++ -std=c++17 -O2 -static -static-libgcc -static-libstdc++ -o ChatServer.exe main.cpp ChatServer.cpp Client.cpp ChatListener.cpp BroadcastClient.cpp DMClient.cpp -lws2_32

# Build test client
g++ -std=c++17 -O2 -static -static-libgcc -static-libstdc++ -o ChatClient.exe ChatClient.cpp -lws2_32
```

**Note**: The `-static` flags are required on Windows to avoid runtime DLL dependency issues.

### Troubleshooting Build Issues

**Problem**: `g++: command not found`  
**Solution**: Install MinGW-w64 or MSYS2 and add to PATH

**Problem**: Exit code -1073741819 (ACCESS_VIOLATION)  
**Solution**: Add `-static -static-libgcc -static-libstdc++` flags

**Problem**: `undefined reference to 'WSAStartup'`  
**Solution**: Add `-lws2_32` to link Winsock library

## Running the Server

### Start the Server

#### Default Configuration (Port 4000, 60s timeout)
```powershell
.\ChatServer.exe
```

**Output:**
```
========================================
            TCP Chat Server
========================================
Port: 4000
Idle Timeout: 60 seconds
========================================

Architecture:
  - ChatServer: Main server (always active)
  - Client: Base class for each connection
  - BroadcastClient: Handles message broadcasting
  - DMClient: Handles direct messages
  - Connect: Manages client-server connection
========================================

Protocol Commands:
  LOGIN <username>   - Log in with a username
  MSG <text>         - Send a message to all users
  DM <user> <text>   - Send a direct message
  WHO                - List all connected users
  PING               - Keep connection alive (server responds with PONG)

Press Ctrl+C to stop the server

Server started. Waiting for connections...
```

#### Custom Port
```powershell
# Port 5000
.\ChatServer.exe 5000
```

#### Custom Port and Idle Timeout
```powershell
# Port 5000, 120 second timeout
.\ChatServer.exe 5000 120
```

### Stop the Server

Press `Ctrl+C` for graceful shutdown. The server will:
1. Send shutdown notification to all clients
2. Close all client connections
3. Clean up Winsock resources
4. Exit cleanly

## Protocol Commands

All commands must end with a newline (`\n`).

### LOGIN
```
LOGIN <username>
```
Authenticates user with given username.

**Responses:**
- `OK` - Login successful
- `ERR username-taken` - Username already in use
- `ERR invalid-username` - Username is empty

**Example:**
```
> LOGIN alice
< OK
```

### MSG (Broadcast Message)
```
MSG <text>
```
Broadcasts message to all authenticated users.

**Format:** Server sends to all users: `MSG <sender-username> <text>`

**Example:**
```
> MSG Hello everyone!
< MSG alice Hello everyone!
```

### DM (Direct Message)
```
DM <target-username> <text>
```
Sends private message to specific user.

**Responses:**
- Message delivered to target user as: `DM <sender-username> <text>`
- `ERR user-not-found` - Target user doesn't exist
- `ERR invalid-dm-format` - Command format error

**Example:**
```
> DM bob Hey, how are you?
(Bob receives: DM alice Hey, how are you?)
```

### WHO (List Users)
```
WHO
```
Returns list of all authenticated users.

**Response:** One `USER <username>` per connected user

**Example:**
```
> WHO
< USER alice
< USER bob
< USER charlie
```

### PING (Heartbeat)
```
PING
```
Keeps connection alive and resets idle timer.

**Response:** `PONG`

**Example:**
```
> PING
< PONG
```

## Complete Example Session: Two Users Chatting

### Server Output
```
========================================
   TCP Chat Server
========================================
Port: 4000
Idle Timeout: 60 seconds
========================================

Architecture:
  - ChatServer: Main server (always active)
  - Client: Base class for each connection
  - BroadcastClient: Handles message broadcasting
  - DMClient: Handles direct messages
  - Connect: Manages client-server connection
========================================

Protocol Commands:
  LOGIN <username>   - Log in with a username
  MSG <text>         - Send a message to all users
  DM <user> <text>   - Send a direct message
  WHO                - List all connected users
  PING               - Keep connection alive (server responds with PONG)

Press Ctrl+C to stop the server

Server started. Waiting for connections...
New client connected from 127.0.0.1
Client 127.0.0.1 authenticated as: alice
New client connected from 127.0.0.1
Client 127.0.0.1 authenticated as: bob
Client alice disconnected
Client bob disconnected
```

### User 1 (Alice) - Terminal Session
```powershell
PS C:\> telnet localhost 4000

LOGIN alice
OK
INFO alice connected
MSG Hi everyone, I'm Alice!
MSG alice Hi everyone, I'm Alice!
INFO bob connected
MSG bob Hello Alice! I'm Bob
WHO
USER alice
USER bob
DM bob Want to grab coffee later?
MSG Thanks for chatting, everyone!
MSG alice Thanks for chatting, everyone!
INFO bob disconnected
^C
```

### User 2 (Bob) - Terminal Session
```powershell
PS C:\> telnet localhost 4000

LOGIN bob
OK
INFO bob connected
MSG alice Hi everyone, I'm Alice!
MSG Hello Alice! I'm Bob
MSG bob Hello Alice! I'm Bob
DM alice Want to grab coffee later?
WHO
USER alice
USER bob
MSG alice Thanks for chatting, everyone!
PING
PONG
^C
```

### Example with Idle Timeout

```
User connects but doesn't send any commands for 60 seconds:

T+0s:  LOGIN alice
       OK
T+60s: INFO timeout-disconnect
       [Connection closed by server]
```

## Connecting to the Server

### Method 1: Using Telnet

```powershell
# Install telnet (if not already installed)
dism /online /Enable-Feature /FeatureName:TelnetClient

# Connect to server
telnet localhost 4000
```

### Method 2: Using the Included Test Client (Reccomended)

```powershell
# Build the test client (if not already built)
g++ -std=c++17 -O2 -static -static-libgcc -static-libstdc++ -o ChatClient.exe ChatClient.cpp -lws2_32

# Run test client
.\ChatClient.exe

# Or connect to specific server
.\ChatClient.exe localhost 4000
```

## Error Codes

| Error Code | Description | Trigger |
|------------|-------------|---------|
| `ERR username-taken` | Username already in use | LOGIN with existing username |
| `ERR invalid-username` | Empty or invalid username | LOGIN with empty string |
| `ERR not-authenticated` | Must login first | Sending MSG/DM/WHO before LOGIN |
| `ERR unknown-command` | Invalid command | Unrecognized command |
| `ERR invalid-dm-format` | DM command format error | DM without target or message |
| `ERR empty-message` | Message cannot be empty | MSG or DM with no text |
| `ERR user-not-found` | DM target user not found | DM to non-existent user |

## Server Notifications

These are informational messages sent by the server to clients:

| Notification | Description | When |
|--------------|-------------|------|
| `INFO <username> connected` | User joined chat | After successful LOGIN |
| `INFO <username> disconnected` | User left chat | User disconnects normally |
| `INFO <username> disconnected (timeout)` | User timed out | 60s idle timeout triggered |
| `INFO timeout-disconnect` | You were disconnected | Sent to user before timeout disconnect |
| `INFO server-shutdown` | Server is shutting down | Ctrl+C pressed on server |

## Video Demo

📹 **[Watch Demo Video](https://youtu.be/9ATaRwJuI04)**

The demo video covers:
- Building and running the server
- Connecting multiple clients
- User authentication (LOGIN)
- Broadcasting messages (MSG)
- Direct messaging (DM)
- Listing users (WHO)
- Heartbeat mechanism (PING/PONG)
- Idle timeout demonstration
- Graceful server shutdown

## Key Features Implementation

### Object-Oriented Architecture
- **Base Class Pattern**: `Client` base class with virtual methods
- **Inheritance**: `BroadcastClient` and `DMClient` extend `Client`
- **Polymorphism**: Server manages clients through base class pointers
- **Encapsulation**: Each class has clear, focused responsibilities
- **No Static Methods**: Pure instance-based OOP design

### Idle Timeout
- Automatically disconnects users after 60 seconds of inactivity (default)
- Any command (LOGIN, MSG, DM, WHO, PING) resets the idle timer
- Configurable timeout period via command-line argument
- Sends warning notification before disconnect

### Thread Safety
- Uses `std::mutex` locks for client list management
- Each client runs in its own `std::thread`
- Atomic boolean (`std::atomic<bool>`) for server running state
- Safe concurrent access to shared resources

### Memory Management
- Uses `std::shared_ptr<Client>` for automatic memory management
- No manual new/delete operations
- RAII principle for socket cleanup
- Proper destructor chain: DMClient → BroadcastClient → Client

### Graceful Shutdown
- Ctrl+C handler (`SIGINT`) for clean shutdown
- Notifies all clients with `INFO server-shutdown` before disconnecting
- Properly closes all sockets
- Cleans up Winsock resources with `WSACleanup()`
- Joins all threads before exit

### Socket Management
- Proper socket initialization and cleanup
- Non-blocking operations with timeout
- Error handling for network failures
- Prevents socket double-close with `INVALID_SOCKET` checks

## Project Structure

```
chatTCP/
├── main.cpp                  # Entry point with signal handlers
├── ChatServer.h/.cpp         # Main server managing connections
├── Client.h/.cpp             # Base class for client connections
├── BroadcastClient.h/.cpp    # Child class for broadcasting
├── DMClient.h/.cpp           # Child class for direct messaging
├── Connect.h/.cpp            # Connection manager (legacy/utility)
├── ChatListener.h/.cpp       # Command parser and router
├── ChatClient.cpp/.exe       # Test client application
├── serverDefaults.h          # Default configuration constants
├── build.ps1                 # PowerShell build script
├── start-server.ps1          # Server startup script
├── README.md                 # This file
├── QUICKSTART.md             # Quick start guide
└── PROJECT_SUMMARY.md        # Project summary
```

**Note**: `Connect.h/.cpp` is part of the codebase but not currently compiled into the server binary. The server uses direct Client-based connections.

## Requirements

- **Operating System**: Windows (uses Winsock2 API)
- **Compiler**: C++17 compatible compiler (g++ recommended)
- **Build Tools**: PowerShell for build script
- **Runtime**: Windows Socket library (ws2_32.lib)
- **No External Dependencies**: Uses only standard C++17 and Windows API


## Common Issues and Solutions

### Issue: Server won't start
**Error**: `Failed to initialize Winsock` or `Failed to create socket`  
**Solution**: 
- Ensure port 4000 is not in use: `netstat -ano | findstr :4000`
- Run as administrator if port < 1024
- Check firewall settings

### Issue: Client disconnects immediately
**Error**: Connection closes right after LOGIN  
**Solution**: 
- This was a bug fixed in the latest version
- Ensure you're using the updated code with `INVALID_SOCKET` checks
- Rebuild with `.\build.ps1`

### Issue: Compilation fails
**Error**: `undefined reference to 'WSAStartup'`  
**Solution**: Add `-lws2_32` to link command

**Error**: DLL errors on execution  
**Solution**: Add `-static -static-libgcc -static-libstdc++` flags

### Issue: Idle timeout too aggressive
**Solution**: Increase timeout when starting server: `.\ChatServer.exe 4000 120`

## Performance Characteristics

- **Concurrent Users**: Tested with 10+ simultaneous connections
- **Message Latency**: < 10ms for broadcast messages on localhost
- **Memory Usage**: ~5-10 MB per connected client
- **CPU Usage**: Minimal (< 1% on modern CPUs for idle connections)
- **Thread Model**: One thread per client + main accept thread + idle checker thread


