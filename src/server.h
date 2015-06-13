//
// Created by 戎 佳磊 on 15/6/12.
//

#ifndef SRC_SERVER_H
#define SRC_SERVER_H
#include <stdio.h>
#ifndef MAXCONNECTION
#define MAXCONNECTION 10000
#endif
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
class RunServer{
public:
    void AccpetConnect();
private:
    struct sockaddr_in server;
    int sockfd;
    char ip[INET6_ADDRSTRLEN];
    char ipc[INET6_ADDRSTRLEN];

};
#endif //SRC_SERVER_H
