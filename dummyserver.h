#ifndef DUMMYSERVER_H
#define DUMMYSERVER_H


#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/ip.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h> // htons, ntohs ...
#include <sys/epoll.h>

#include <errno.h>
#include <cstring>    // memset, memcpy
#include <cstddef>    // ptrdiff_t
#include <iostream>
#include <vector>

#include <sstream>
#include <iomanip>



//for debug
#include <stdio.h>


#include "settings.h"
#include "utils.h"
#include "CmdS7/s7common.h"
#include "DBhandler/dbhandler.h"
#include "CmdS7/cmdhandler.h"


namespace FPLC
{

/*
 * Simple one-thread tcp server.
 *
 */

class DummyServer
{
    public:
        DummyServer(S7Cmd::CmdHandler* p);
        ~DummyServer();
        int run(uint16_t port);

        void setCmdHandler(std::shared_ptr<S7Cmd::CmdHandler> & p);
        
        DummyServer(const DummyServer &) = delete;
        DummyServer & operator=(const DummyServer &) = delete;
    
    private:
        int checkSock(int sd);
        int checkBind(int ad);
        int checkListen(int ld);
        int setnonblocking(int sockfd);

        const size_t BUF_SIZE = 65535; 
        S7Cmd::CmdHandler* cmdHandler;
};


}//namespace
#endif // DUMMYSERVER_H
