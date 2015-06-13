//
// Created by 戎 佳磊 on 15/6/12.
//

#ifndef SRC_HEADER_FILE_H
#define SRC_HEADER_FILE_H
#include "praser.h"
#include "server.h"
extern pthread_mutex_t rqueue_lock;
extern pthread_cond_t rqueue_cond;
extern pthread_mutex_t print_lock;
extern pthread_cond_t print_cond;
extern Parser *P;
extern int sockId;
extern bool debug_mode;
extern RunServer * run;
extern std::string port;
extern bool logging;
extern std::string scheduling;
extern int threadnum;
extern bool summary;
extern int r_time;
extern std::string l_file;
extern std::string rootdir;
extern std::string log_root;
extern std::string ipaddr;
#endif //SRC_HEADER_FILE_H
