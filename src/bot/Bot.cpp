#include "../../include/Bot.hpp"

const char* BAD_WORDS[] = {"hmar", "kelb", "fuck", "nigga", NULL};

bool Bot::parseArg(int ac, char **av, int &port, std::string &password, std::string &channel)
{
    if (ac != 4) {
        std::cerr << "Error: Invalid number of arguments." << std::endl;
        std::cerr << "Usage: ./bot <port> <password> <channel>" << std::endl;
        return false;
    }

    std::string portStr = av[1];
    password = av[2];
    channel = av[3];

    if (portStr.empty()) {
        std::cerr << "Error: Port cannot be empty." << std::endl;
        return false;
    }

    for (size_t i = 0; i < portStr.length(); ++i) {
        if (!isdigit(portStr[i])) {
            std::cerr << "Error: Port must be a numeric value." << std::endl;
            return false;
        }
    }

    char *endPtr;
    long val = std::strtol(portStr.c_str(), &endPtr, 10);

    if (*endPtr != '\0') {
        std::cerr << "Error: Invalid characters in port." << std::endl;
        return false;
    }

    if (val < 0 || val > 65535) {
        std::cerr << "Error: Port out of valid range (0-65535)." << std::endl;
        return false;
    }

    port = static_cast<int>(val);

    if (password.empty()) {
        std::cerr << "Error: Password cannot be empty." << std::endl;
        return false;
    }

    if (channel.empty()) {
        std::cerr << "Error: Channel cannot be empty." << std::endl;
        return false;
    }

    for (size_t i = 0; i < channel.length(); ++i) {
        if (!isalnum(channel[i])) {
            std::cerr << "Error: Channel must be alphanumeric." << std::endl;
            return false;
        }
    }

    return true;
}

void Bot::sendData(int socket, std::string msg) {
    msg += "\r\n";
    send(socket, msg.c_str(), msg.length(), 0);
    std::cout << "[SENT] " << msg;
}

bool Bot::containsBadWord(std::string message) {
    for (size_t i = 0; i < message.length(); ++i)
        message[i] = tolower(message[i]);

    for (int i = 0; BAD_WORDS[i] != NULL; i++) {
        if (message.find(BAD_WORDS[i]) != std::string::npos)
            return true;
    }
    return false;
}


std::string Bot::extractNick(std::string prefix) {
    size_t end = prefix.find('!');
    if (end != std::string::npos)
        return prefix.substr(1, end - 1);
    return "";
}

int main(int ac, char **av) {

    Bot bot;

    if (!Bot::parseArg(ac, av, bot.port, bot.password, bot.channel)) {
        return 1;
    }


    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(bot.port);
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }
    std::cout << "Connected to server..." << std::endl;

    Bot::sendData(sock, "PASS " + bot.password);
    Bot::sendData(sock, "NICK " + std::string(BOT_NICK));
    Bot::sendData(sock, "USER 7med * 0 :li 3waj ngadoh");


    Bot::sendData(sock, "JOIN #" + bot.channel);

    char buffer[4096];
    std::string incomplete_msg = "";

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes <= 0) break;

        std::string raw_data = incomplete_msg + buffer;
        incomplete_msg = "";

        size_t pos = 0;
        size_t next_pos;

        while ((next_pos = raw_data.find("\r\n", pos)) != std::string::npos) {
            std::string line = raw_data.substr(pos, next_pos - pos);
            pos = next_pos + 2;

            if (line.find("PING") == 0) {
                std::string pong = "PONG" + line.substr(4);
                Bot::sendData(sock, pong);
            }

            if (line.find("PRIVMSG #" + bot.channel) != std::string::npos) {
                
                size_t msg_start = line.find(" :", 0);
                if (msg_start != std::string::npos) {
                    std::string message = line.substr(msg_start + 2);
                    
                    if (Bot::containsBadWord(message)) {
                        size_t prefix_end = line.find(' ');
                        std::string prefix = line.substr(0, prefix_end);
                        std::string offender = Bot::extractNick(prefix);

                        if (offender != BOT_NICK) {
                            std::cout << "[!] Nari khouna khser lhdra " << offender << std::endl;   
                            Bot::sendData(sock, "KICK #" + bot.channel + " " + offender + " :Hbet Awa hbt!");
                        }
                    }
                }
            }
        }
        if (pos < raw_data.length()) {
            incomplete_msg = raw_data.substr(pos);
        }
    }

    close(sock);
    return 0;
}