//
//  main.cpp
//  MyHttpd
//
//  Created by rongjialei on 5/30/15.
//  Copyright (c) 2015 Rong jialei. All rights reserved.
//

#include <iostream>
#include "header_file.h"
#include <unistd.h>
using namespace std;
pthread_mutex_t rqueue_lock;
pthread_cond_t rqueue_cond;
pthread_mutex_t print_lock;
pthread_cond_t print_cond;
string port = "8080";
bool debug_mode = false;
bool logging = false;
string scheduling ="FCFS";
int threadnum = 10;
bool summary = false;
int r_time = 0;
string ipaddr = "0.0.0.0";
string log_root = "/Users/Charles/ClionProjects/Master/log";
string l_file ="log.txt";
string rootdir ="/Users/Charles/ClionProjects/Master/asset";
Parser * P = new Parser();
pthread_t schedule;
pthread_t thread[30];
int sockId;
RunServer * run = new RunServer;

void printh()
{
    cout<<endl<<"**************************************************************************"<<endl;
    cout<<"Usage Summary: myhttpd -d -h -l filename -p portno -r rootdirectory -t threadwaittime -n threadnumber -s scheduling"<<endl<<endl;
    cout<<"Give -d parameter in args to on debugging mode"<<endl;
    cout<<"Give -h parameter to display the summary"<<endl;
    cout<<"Give -l and then filename of logging file for example: logging.txt"<<endl;
    cout<<"Give -p and then portno to change default port number for example: -p 8080"<<endl;
    cout<<"Give -r and then root directory to change default value of root directory for example: -r /Users/rongjialei/Test"<<endl;
    cout<<"Give -t and then thread time to change default wait time of scheduler thread for example: -t 3"<<endl;
    cout<<"Give -n and then thread numbers to change the default value of threads for example: -n 10"<<endl;
    cout<<"Give -s and then scheduling name to change default scheduling for example: -s SJF"<<endl;
    cout<<"Press Ctrl+c anytime to exit the server";
    cout<<endl<<"**************************************************************************"<<endl;
    exit(0);
}
int main(int argc, const char * argv[]) {
    int opt = 0;
    opt = getopt(argc, (char * const*)argv, "dhl::p:r:t:n:s:i:");
    while( opt != -1 ) {
        switch( opt ) {
            case 'd':
                debug_mode = true;
                break;
            case 'h':
                summary = true;
                break;
            case 'l':
                if(optarg){
                    l_file.assign(optarg);
                }
                logging = true;
                l_file = log_root + l_file;
                break;
            case 'p':
                port.assign(optarg);
                break;
            case 'i':
                ipaddr.assign(optarg);
                break;
            case 'r':
                rootdir.assign(optarg);
                break;
            case 's':
                scheduling.assign(optarg);
                break;
            case 't':
                r_time =atoi(optarg);
                break;
            case 'n':
                threadnum=atoi(optarg);
                break;
            default:
                break;
        }
        opt = getopt(argc, (char * const*)argv, "dhl::p:r:t:n:s:i:");
    }

    if (debug_mode) {
        run->AccpetConnect();
    }
    if (summary) {
        printh();
    }
    else {
        if (pthread_mutex_init(&rqueue_lock, NULL) != 0) {
            printf("\n mutex init failed\n");
            return 0;
        }
        pthread_mutex_init(&print_lock, NULL);
        pthread_cond_init (&rqueue_cond, NULL);
        pthread_cond_init (&print_cond, NULL);
        pthread_create(&schedule,NULL,&Parser::PopRequest_Init,P);

        for(int i =0;i<threadnum;i++)
            pthread_create(&thread[i],NULL,&Parser::ServeRequest_Init,P);
        run->AccpetConnect();
        pthread_mutex_destroy(&rqueue_lock);
        pthread_cond_destroy(&rqueue_cond);
        pthread_mutex_destroy(&print_lock);
        pthread_cond_destroy(&print_cond);
        pthread_exit(NULL);

    }
    close(sockId);
    delete P;
    P = NULL;
    delete run;
    run = NULL;
    return 0;

}
