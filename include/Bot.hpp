#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define BOT_NICK "7med"
#define SERVER_IP "127.0.0.1"

class Bot
{
    public:
        int port;
        std::string password;
        std::string channel;

        static std::string extractNick(std::string prefix);
        static void sendData(int socket, std::string msg);
        static bool parseArg(int ac, char **av, int &port, std::string &password, std::string &channel);
        static bool containsBadWord(std::string message);
};
