//
//  praser.cpp
//  MyHttpd
//
//  Created by rongjialei on 5/30/15.
//  Copyright (c) 2015 Rong jialei. All rights reserved.
//

#include "praser.h"
#include <sys/fcntl.h>
#include "sender.h"
#include <unistd.h>
char ERROR_Req[] = "Error:Bad Request,Retry";
using namespace std;
void  Parser::Debug(ClientInfo c_info){
    cout << "\033[1;31mbold red text\033[0m\n";
    cout << "DEBUG MODE\n";
    cout << c_info.protocalType<<'\n';
    cout << c_info.Method <<'\n';
    cout << c_info.reqtime <<'\n';
    cout << c_info.ip_addr <<'\n';
    cout << c_info.n_port <<'\n';
    cout << c_info.firstline <<'\n';
    cout << c_info.filename <<'\n';
    cout << "\033[1;31mbold red text\033[0m\n";
    cout <<"DEBUG END\n";
}
int Parser::RootCheck(string filePath){
    int flag = 0;
    char * p = strtok((char *)filePath.c_str(), "/");
    while(p != NULL){
        if(strcmp(p, "..") == 0)
        {
            --flag;
            if(flag < 0){
                return flag;
            }
        }
        else{
            ++flag;
        }
        p = strtok(NULL, "/");
    }
    return flag;
}
void Parser::IsUserDir(string & filepath ){
    if(filepath[1] == '~'){
        filepath.erase(1,1);
    }
}
void Parser::Parse_Request(ClientIdentity cid){
    int i = 1;
    ssize_t recvbytes = 0;
    char buff[1024];
    ClientInfo c_info;
    fcntl( cid.sockfd, O_NONBLOCK, 0 ); //Non-blocking I/O; if no data is available to a read call, or if a write operation would block, the read or write call returns -1 with the error EAGAIN.
    if((recvbytes = (recv(cid.sockfd, buff, sizeof(buff),0))) == -1)
        perror("Receive:");
    buff[recvbytes] = '\0';

    string convertLine(buff);
    c_info.buf = convertLine;
    size_t pos = 0;
    size_t next = convertLine.find_first_of("\r\n",pos);
    if (next == std::string::npos) {
        send(cid.sockfd, ERROR_Req, strlen(ERROR_Req), 0);
        close(cid.sockfd);
    }
    else{
        c_info.firstline = convertLine.substr(pos,next - pos);
        string HeaderParseString[3];
        char * p;
        p = strtok((char *)c_info.firstline.c_str(), " ");
        HeaderParseString[0].assign(p);
        while (i < 3) {
            p = strtok(NULL," ");
            HeaderParseString[i].assign(p);
            ++i;
        }
        c_info.filename = HeaderParseString[1];
        this->IsUserDir(c_info.filename);
        if(RootCheck(c_info.filename) < 0){
            c_info.rootcheck = false;
        }
        else{
            c_info.rootcheck = true;
        }
        c_info.sockfd = cid.sockfd;
        c_info.protocalType = HeaderParseString[2];
        c_info.Method = HeaderParseString[0];
        c_info.filename = rootdir + c_info.filename;
        c_info.ip_addr = cid.ip_addr;
        c_info.n_port = cid.n_port;
        c_info.reqtime = cid.request_time;
        CheckRequest(c_info);
    }
}

void Parser::CheckRequest(ClientInfo c_info){
    size_t pos = c_info.filename.find_last_of("/");
    size_t next = c_info.filename.find_first_of(".",pos);
    if (next != string::npos) {
        if (FileExist((char *)c_info.filename.c_str()) == true && c_info.Method == "GET") {
            FILE * pfile;
            size_t s;
            pfile = fopen(c_info.filename.c_str(), "r");
            if(pfile ==NULL){
                c_info.file_status = false;
                c_info.file_size = 0;
                perror("open file");
            }
            else{
                fseek(pfile, 0, SEEK_END);
                s = ftell(pfile);
                c_info.file_size = (int)s;
                c_info.file_status = true;
            }
            fclose(pfile);
            if (debug_mode) {
                Sender s;
                s.SendData(c_info);
            }
            else{
                this->ReadyQueue(c_info);
            }
        }
        else if(FileExist((char *) c_info.filename.c_str()) == true && c_info.Method == "HEAD"){
            c_info.file_size = 0;
            c_info.file_status = true;
            if (debug_mode) {
                Sender s;
                s.SendData(c_info);
            }
            else{
                this->ReadyQueue(c_info);
            }
        }
        else {
            c_info.file_size = 0;
            c_info.file_status = false;
            if (debug_mode) {
                Sender s;
                s.SendData(c_info);
            }
            else{
                this->ReadyQueue(c_info);
            }
        }
    }
    else{
        c_info.file_size = 0;
        c_info.file_status = false;
        if (debug_mode) {
            Sender s;
            s.SendData(c_info);
        }
        else{
            this->ReadyQueue(c_info);
        }
    }
}
bool Parser::FileExist(char * filename){
    struct stat fs;
    int ret = stat(filename ,&fs);
    if (ret == -1) {
        return false;
    }
    else
        return true;
}
// base on file size
bool SortRequest(const ClientInfo &c1 ,const ClientInfo & c2){
    return c1.file_size < c2.file_size;
}
void Parser::ReadyQueue(ClientInfo c_info){
    pthread_mutex_lock(&rqueue_lock);
    ClientList.push_back(c_info);
    pthread_cond_signal(&rqueue_cond);
    pthread_mutex_unlock(&rqueue_lock);
}

void Parser::PopRequest(){
    sleep((unsigned int)r_time);
    ClientInfo c_info;
    transform(scheduling.begin(), scheduling.end(), scheduling.begin(),::toupper);
    int flag = 0;
    if(scheduling == "SJF")flag = 1;
    while (1) {
        if (flag == 1) {
            pthread_mutex_lock(&rqueue_lock);
            while (ClientList.empty()) {
                pthread_cond_wait(&rqueue_cond, &rqueue_lock);
            }
            ClientList.sort(SortRequest);
            c_info = ClientList.front();
            ClientList.pop_front();
            pthread_mutex_unlock(&rqueue_lock);
        }
        else{
            pthread_mutex_lock(&rqueue_lock);
            while (ClientList.empty()) {
                pthread_cond_wait(&rqueue_cond, &rqueue_lock);
            }
            c_info = ClientList.front();
            ClientList.pop_front();
            pthread_mutex_unlock(&rqueue_lock);
        }
        pthread_mutex_lock(&print_lock);
        ReqList.push_back(c_info);
        pthread_cond_signal(&print_cond);
        pthread_mutex_unlock(&print_lock);

    }
}
void *Parser::PopRequest_Init(void * arg){
    ((Parser*)arg)->PopRequest();
    return NULL;
}
void Parser::ServeRequest(){
    pthread_detach(pthread_self()); //detach child thread from main thread
    while (1) {
        pthread_mutex_lock(&print_lock);
        while (ReqList.empty()) {
            pthread_cond_wait(&print_cond, &print_lock);
        }
        Sender s;
        ClientInfo c;
        c = ReqList.front();
        ReqList.pop_front();
        time_t t;
        time(&t);
        struct tm * now;
        now  = gmtime(&t);
        char cur[50];
        strftime(cur, 50, "%x:%X", now); //Date Time
        c.sevtime = string(cur);
        pthread_mutex_unlock(&print_lock);
        s.SendData(c);
    }
}
void * Parser::ServeRequest_Init(void * arg){
    Parser * p = (Parser * )arg;
    p->ServeRequest();
    return NULL;
}



