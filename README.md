# ft_irc

A minimal IRC server written in **C++98** (42 School project).

## Features (implemented)

- TCP server using sockets (`bind` / `listen` / `accept`)
- Multiple clients
- Basic IRC command handling (see `src/commands/`)
- Channels management

## Requirements

- `c++` compiler supporting **C++98**
- Unix-like environment (Linux/macOS)

## Build

```bash
make
```

This produces the server binary:

- `./ircserv`

### Build the bot (optional)

```bash
make bot
```

This produces:

- `./7med_lgzar`

## Usage

Run the server:

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 hunter2
```

## Connect with an IRC client

Using `nc` (quick testing):

```bash
nc 127.0.0.1 6667
```

Typical handshake (example):

```text
PASS hunter2
NICK mynick
USER myuser 0 * :My Real Name
```

Or use a real IRC client (HexChat, irssi, WeeChat) and connect to:

- Host: `127.0.0.1`
- Port: `<port>`
- Server password: `<password>`

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
│       ├── User.cpp
└── Makefile
```

## Server / Client Workflow (high level)

| Step | Server Action                   | Client Action             |
| ---- | ------------------------------- | ------------------------- |
| 1    | Create socket endpoint          | Create socket endpoint    |
| 2    | Bind to IP and port             | —                         |
| 3    | Listen for incoming connections | —                         |
| 4    | Accept client connection        | Connect to server IP/port |
| 5    | Send / receive messages         | Send / receive messages   |

## Notes

This repository targets the IRC protocol behavior required by the 42 `ft_irc` subject, and is compiled with strict flags:

- `-Wall -Wextra -Werror -std=c++98`

## License

If you want, add a license section here (MIT/Unlicense/etc.).
