//
//  sender.cpp
//  MyHttpd
//
//  Created by rongjialei on 5/30/15.
//  Copyright (c) 2015 Rong jialei. All rights reserved.
//

#include "sender.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fstream>
#include <time.h>
#include <pthread.h>
#include <sstream>
#include <unistd.h>
#ifndef st_mtime
#define st_mtime st_mtimespec.tv_sec
#endif
char NotFound[] = "Error 404 Not Found";
char ReqForbidden[] = "ReqForbidden";

using namespace std;
void Sender::SendData(ClientInfo c_info){
    char curTime[50];
    char lastModify[50];
    struct stat fs;
    if (c_info.file_status == true  && c_info.Method == "GET") {
        time_t rawTime;
        struct tm * ptm;
        time(&rawTime);
        ptm = gmtime(&rawTime);
        if (strftime(curTime, 50, "%c", ptm) == 0) {
            perror("StrftTime");
        }
        char * filename = new char[c_info.filename.size() + 1];
        copy(c_info.filename.begin(), c_info.filename.end(), filename);
        filename[c_info.filename.size()] = '\0';
        stat(filename, &fs);
        strcpy(lastModify, ctime(&fs.st_mtime));
        delete []filename;
        size_t pos = c_info.filename.find_last_of("/");
        size_t id = c_info.filename.find_first_of(".",pos+1);
        string c_type = c_info.filename.substr(id+1 ,c_info.filename.size());
        transform(c_type.begin(), c_type.end(), c_type.begin(), ::tolower);
        if (c_type == "txt" || c_type == "html" || c_type == "htm" ) {
            c_info.ctype = "text/html";
        }
        else if(c_type == "css" )c_info.ctype = "text/css";
        else if(c_type == "js" )c_info.ctype= "text/js";
        else if(c_type == "gif" || c_type == "jpeg" || c_type == "jpg" || c_type == "ico"){
            c_info.ctype =  "image/"+ c_type;
        }
        else
            c_info.ctype ="";
        std::stringstream ss;
        ss << c_info.file_size;
        string filesize = ss.str();
        string header = c_info.protocalType + " "+ "200 OK" + "\r\n";
        header += "Date: "+ string(curTime) + "\r\n" + "Server :myHttpd\r\n" + "Last-Modified: " + string(lastModify)  + "Content-Type:" + c_info.ctype + "\r\n" + "Content-Length:" + filesize + "\r\n\r\n";

        cout <<c_info.buf  +  header  << endl;
        if (send(c_info.sockfd, header.c_str(), header.length(), 0) == -1) {
            perror("send header");
        }

        char * fileBlock;
        FILE * pfile;
        size_t s;
        pfile = fopen(c_info.filename.c_str(), "r");
        if (pfile ==NULL) {
            perror("FILE OPEN");
        }
        else{
            fseek(pfile, 0, SEEK_END);
            s = ftell(pfile);
            fseek(pfile, 0, SEEK_SET);
            fileBlock = new char[s];
            fread(fileBlock, 1, s, pfile);
        }
        if (send(c_info.sockfd, fileBlock, s, 0) == -1) {
            perror("send");
        }
        fclose(pfile);
        delete [] fileBlock;

        c_info.status_code = 200;
    }
    else if(c_info.file_status == true && c_info.Method == "HEAD"){
        time_t rawTime;
        struct tm * ptm;
        time(&rawTime);
        ptm = gmtime(&rawTime);
        if (strftime(curTime, 50, "%c", ptm) == 0) {
            perror("StrftTime");
        }
        char * filename = new char[c_info.filename.size() + 1];
        copy(c_info.filename.begin(), c_info.filename.end(), filename);
        filename[c_info.filename.size()] = '\0';
        stat(filename, &fs);
        strcpy(lastModify, ctime(&fs.st_mtime));
        delete []filename;
        size_t pos = c_info.filename.find_last_of("/");
        string c_type = c_info.filename.substr(pos+1 ,c_info.filename.size());
        if (c_type == "txt" || c_type == "html" || c_type == "htm") {
            c_info.ctype = "text/html";
        }
        else if(c_type == "css")c_info.ctype = "text/css";
        else if(c_type == "js")c_info.ctype = "text/js";
        else if(c_type == "gif" || c_type == "jpeg" || c_type =="ico"){
            c_info.ctype =  "image/"+ c_type;
        }
        else
            c_info.ctype ="";
        std::stringstream ss;
        ss << c_info.file_size;
        string filesize = ss.str();
        string header = c_info.protocalType + " "+ "200 OK" + "\r\n";
        header += "Date: "+ string(curTime) + "\r\n" + "Server :myHttpd\r\n" + "Last-Modified: " + string(lastModify)  + "Content-Type:" + c_info.ctype + "\r\n" + "Content-Length:" + filesize + "\r\n\r\n";
        cout <<c_info.buf +  header  << endl;
        if (send(c_info.sockfd,header.c_str(), header.length(),0) ==-1) {
            perror("Send");
        }
        c_info.status_code = 200;
    }
    else{
        if(c_info.rootcheck == false){
            char * fileBlock;
            FILE * pfile;
            size_t s;
            pfile = fopen("/Users/Charles/ClionProjects/Master/asset/notfound.html", "r");
            if (pfile ==NULL) {
                perror("FILE OPEN");
            }
            else{
                fseek(pfile, 0, SEEK_END);
                s = ftell(pfile);
                fseek(pfile, 0, SEEK_SET);
                fileBlock = new char[s];
                fread(fileBlock, 1, s, pfile);
            }
            if (send(c_info.sockfd, fileBlock, s, 0) == -1) {
                perror("send");
            }
            fclose(pfile);
            delete [] fileBlock;
            c_info.status_code = 403;

            cout << c_info.buf  + ReqForbidden << endl;
        }
        else{
            char * fileBlock;
            FILE * pfile;
            size_t s;
            pfile = fopen("/Users/Charles/ClionProjects/Master/asset/notfound.html", "r");
            if (pfile ==NULL) {
                perror("FILE OPEN");
            }
            else{
                fseek(pfile, 0, SEEK_END);
                s = ftell(pfile);
                fseek(pfile, 0, SEEK_SET);
                fileBlock = new char[s];
                fread(fileBlock, 1, s, pfile);
            }
            if (send(c_info.sockfd, fileBlock, s, 0) == -1) {
                perror("send");
            }
            fclose(pfile);
            delete [] fileBlock;
            c_info.status_code = 404;
            cout << c_info.buf  + NotFound << endl;
        }
    }
    if(logging)this->GenerateLogging(c_info);
    if (debug_mode)this->DisplayLog(c_info);
    close(c_info.sockfd);
}

void Sender::GenerateLogging(ClientInfo c_info){
    FILE * pfile;
    pfile = fopen(l_file.c_str(), "a+");
    if (pfile == NULL) {
        perror("Open Logfile");
    }
    else{
        stringstream ss;
        ss << c_info.ip_addr << "\t" <<c_info.n_port <<"\t" << c_info.reqtime<< "\t"<< c_info.sevtime <<"\t"<< c_info.firstline<<"\t" << c_info.status_code<<"\t" << c_info.file_size << "\n";
        fwrite((ss.str()).c_str(), 1, ss.str().length(), pfile);
    }
    fclose(pfile);
}
void Sender::DisplayLog(ClientInfo c_info){
    time_t t;
    time(&t);
    struct tm * now ;
    now = gmtime(&t);
    char curtime[50];
    if (strftime(curtime, 50, "%x:%X", now)  == 0) {
        perror("Time");
    }
    string servertime(curtime);
    c_info.sevtime = servertime;
    cout << c_info.ip_addr << "\t" << c_info.reqtime<< "\t"<< c_info.sevtime <<"\t"<< c_info.firstline<<"\t" << c_info.status_code<<"\t" << c_info.file_size << "\n";
}