## Usage

```bash
./ft_irc <port> <password>
```

## Project Structure

```
.
├── include
│   ├── Channel.hpp      # Channel class declaration
│   ├── Client.hpp       # Client class declaration
│   ├── Parser.hpp       # Parser class declaration
│   ├── Server.hpp       # Server class declaration
│   └── common.hpp       # Shared constants/utilities
├── src
│   ├── Channel.cpp      # Channel implementation
│   ├── Client.cpp       # Client implementation
│   ├── Parser.cpp       # Parser implementation
│   ├── Server.cpp       # Server implementation
│   ├── Server_utils.cpp # Server helpers (socket ops, replies, etc.)
│   ├── execute.cpp      # Command dispatcher/loop
│   ├── main.cpp         # Server bootstrap
│   └── commands         # IRC command handlers
│       ├── Cap.cpp
│       ├── Invite.cpp
│       ├── Join.cpp
│       ├── Kick.cpp
│       ├── Mode.cpp
│       ├── Nick.cpp
│       ├── Part.cpp
│       ├── Pass.cpp
│       ├── Ping.cpp
│       ├── Privmsg.cpp
│       ├── Quit.cpp
│       ├── Topic.cpp
│       └── User.cpp
├── Makefile             # Build rules
└── README.md            # Project overview
```


## Server / Client Workflow

| Step | Server Action                   | Client Action             |
| ---- | ------------------------------- | ------------------------- |
| 1    | Create socket endpoint          | Create socket endpoint    |
| 2    | Bind to IP and port             | —                         |
| 3    | Listen for incoming connections | —                         |
| 4    | Accept client connection        | Connect to server IP/port |
| 5    | Send / receive messages         | Send / receive messages   |
