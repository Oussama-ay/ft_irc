## Usage

```bash
./ft_irc <port> <password>
```

## Project Structure

```
.
├── include
│   ├── Server.hpp   # Server class declaration
│   └── Client.hpp   # Client class declaration
├── src
│   ├── Server.cpp   # Server implementation
│   ├── Client.cpp   # Client implementation
│   └── main.cpp     # server bootstrap
└── Makefile         # Build rules
```

## Server / Client Workflow

| Step | Server Action                   | Client Action             |
| ---- | ------------------------------- | ------------------------- |
| 1    | Create socket endpoint          | Create socket endpoint    |
| 2    | Bind to IP and port             | —                         |
| 3    | Listen for incoming connections | —                         |
| 4    | Accept client connection        | Connect to server IP/port |
| 5    | Send / receive messages         | Send / receive messages   |
