#include "Parser.hpp"

bool Parser::parseArguments(int ac, char **av, int &port, std::string &password) {
    // hadi rah bayna dyalach
    if (ac != 3) {
        std::cerr << "Error: Invalid number of arguments." << std::endl;
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return false;
    }

    std::string portStr = av[1]; // Port
    password = av[2]; // password

    if (portStr.empty()) {
        std::cerr << "Error: Port cannot be empty." << std::endl;
        return false;
    }

    //khas lport ykon num
    for (size_t i = 0; i < portStr.length(); ++i) {
        if (!isdigit(portStr[i])) {
            std::cerr << "Error: Port must be a numeric value." << std::endl;
            return false;
        }
    }

    char *endPtr;
    long val = std::strtol(portStr.c_str(), &endPtr, 10);

    // ports range mn 0 - 65535
    if (val < 0 || val > 65535) {
        std::cerr << "Error: Port out of valid range (0-65535)." << std::endl;
        return false;
    }

    port = static_cast<int>(val);

    if (password.empty()) {
        std::cerr << "Error: Password cannot be empty." << std::endl;
        return false;
    }

    return true;
}


// hadi lfunction li radi yb9a ytsift liha dak lbuffer li wsl breciv 
std::vector<Command> Parser::processBuffer(std::string &buffer) {
    std::vector<Command> commands;
    size_t pos;

    while ((pos = buffer.find("\n")) != std::string::npos) {

        // hadi 7it galk asidi standards dyal RFC 1459 message makhasoch yfot 512 bytes, ana kandropih
        if (pos + 1 > 512) {
            std::cerr << "Error: Message too long (>512 bytes). Dropping message." << std::endl;
            buffer.erase(0, pos + 1);
            continue;
        }

        std::string line = buffer.substr(0, pos);
        
        // handilt l carriage dyal /r ma3reftch wach khas wla la but kayna f standards
        if (!line.empty() && line[line.length() - 1] == '\r') {
            line.erase(line.length() - 1);
        }

        if (!line.empty()) {
            // kanbda nparsi line
            Command cmd = parseLine(line);
            if (cmd.isValid && !cmd.command.empty()) {
                commands.push_back(cmd);
            }
        }

        buffer.erase(0, pos + 1);
    }

    if (buffer.length() > 512) {
        std::cerr << "Error: Incomplete message buffer too long (>512 bytes). Clearing buffer." << std::endl;
        buffer.clear();
    }

    return commands;
}

Command Parser::parseLine(const std::string &line) {
    Command cmd;
    cmd.isValid = true;
    std::stringstream ss(line);
    std::string segment;

    size_t prefixEnd = 0;
    // hna kanjbd l prefix ila kan:
    if (line[0] == ':') {
        prefixEnd = line.find(' ');
        if (prefixEnd != std::string::npos) {
            cmd.prefix = line.substr(1, prefixEnd - 1);
        } else {
            cmd.isValid = false;
            return cmd;
        }
    }

    std::string restOfLine = (prefixEnd == 0) ? line : line.substr(prefixEnd + 1);
    
    size_t currentPos = 0;
    
    while (currentPos < restOfLine.length() && restOfLine[currentPos] == ' ') currentPos++;

    size_t nextSpace = restOfLine.find(' ', currentPos);
    // hna kanjbd l command
    if (nextSpace == std::string::npos) {
        cmd.command = restOfLine.substr(currentPos);
        return cmd;
    }

    cmd.command = restOfLine.substr(currentPos, nextSpace - currentPos);
    currentPos = nextSpace;

    while (currentPos < restOfLine.length()) {
        while (currentPos < restOfLine.length() && restOfLine[currentPos] == ' ') currentPos++;
        
        if (currentPos >= restOfLine.length()) break;

        // hna ila kant dakchi li wra l command badi b : kaykhasni njm3hom f argement whda;
        if (restOfLine[currentPos] == ':') {
            cmd.args.push_back(restOfLine.substr(currentPos + 1));
            break;
        }

        // ila makantch kanbda njm3 whda bwhda fl args;
        nextSpace = restOfLine.find(' ', currentPos);
        if (nextSpace == std::string::npos) {
            cmd.args.push_back(restOfLine.substr(currentPos));
            break;
        } else {
            cmd.args.push_back(restOfLine.substr(currentPos, nextSpace - currentPos));
            currentPos = nextSpace;
        }
    }

    return cmd;
}
