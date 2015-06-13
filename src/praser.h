//
// Created by 戎 佳磊 on 15/6/12.
//

#ifndef SRC_PRASER_H
#define SRC_PRASER_H


#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <vector>

struct ClientIdentity {
    int sockfd;
    std::string ip_addr;
    int n_port;
    std::string request_time;
};
struct ClientInfo{
    std::string protocalType;
    std::string Method;
    std::string firstline;
    std::string filename;
    std::string reqtime;
    std::string sevtime;
    int sockfd;
    std::string ip_addr;
    int n_port;
    int file_size;
    bool file_status;
    std::string ctype;
    std::string buf;
    bool rootcheck;
    int status_code;
};
class Parser{
public:
    std::list<ClientInfo> ClientList;
    std::list<ClientInfo> ReqList;
    bool FileExist(char * filename);
    void Parse_Request(ClientIdentity);
    void ReadyQueue(ClientInfo);
    void CheckRequest(ClientInfo);
    void PopRequest();
    static void * PopRequest_Init(void *);
    void ServeRequest();
    static void * ServeRequest_Init(void *);
    void Debug(ClientInfo);
    int RootCheck(std::string);
    void IsUserDir(std::string &);
};
#endif //SRC_PRASER_H
