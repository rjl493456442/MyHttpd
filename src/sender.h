//
// Created by 戎 佳磊 on 15/6/12.
//

#ifndef SRC_SENDER_H
#define SRC_SENDER_H


#include <stdio.h>
#include <fstream>
#include <sys/types.h>
#include <string>
#include "praser.h"
#include "header_file.h"
class Sender{
public:
    void SendData(ClientInfo);
    void GenerateLogging(ClientInfo);
    void DisplayLog(ClientInfo);

};
#endif //SRC_SENDER_H
