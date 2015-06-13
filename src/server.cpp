//
// Created by 戎 佳磊 on 15/6/12.
//

#include "server.h"
#include <iostream>
#include <list>
#include "header_file.h"
void RunServer::AccpetConnect(){
    struct sockaddr_in Client;
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    int clientfd;
    socklen_t size = sizeof(Client);
    if(sockfd == -1)
    {
        perror("SOCKET");
        exit(0);
    }
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    inet_aton(ipaddr.c_str(),&server.sin_addr);
    server.sin_port = htons(std::stoi(port));
    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind");
        exit(1);
    }
    if (listen(sockfd, MAXCONNECTION) == -1) {
        perror("listen");
        exit(1);
    }
    while (1) {
        if ((clientfd = accept(sockfd, (struct sockaddr*)&Client, &size)) == -1) {
            perror("accpet");
        }
        inet_ntop(AF_INET,&(Client.sin_addr), ip, INET_ADDRSTRLEN);
        time_t t;
        time(&t);
        struct tm * now;
        now = gmtime(&t);
        char cur[50];
        strftime(cur, 50, "%x:%X", now);
        ClientIdentity cid;
        cid.request_time = std::string(cur);
        cid.ip_addr = std::string(ip);
        cid.sockfd = clientfd;
        cid.n_port = Client.sin_port;
        P->Parse_Request(cid);
    }

}