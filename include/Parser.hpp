#ifndef PARSER_HPP
#define PARSER_HPP

#include "common.hpp"

struct Command
{
    std::string prefix;            // prefix means source dyal lmessage like :Aymane it's like katgol Aymane li sift lmessage;
    std::string command;           // bayna
    std::vector<std::string> args; // ila kant ":alo lao" rah kathz kamla ka argement wahd ama ila kant matalan Aymane hello radi tkon ["Aymane", "hello"]
    bool isValid;
};

class Parser
{
public:
    static void parseArguments(char **av, int &port, std::string &password);

    static std::vector<Command> processBuffer(std::string &buffer);

private:
    static Command parseLine(const std::string &line);
};

#endif
