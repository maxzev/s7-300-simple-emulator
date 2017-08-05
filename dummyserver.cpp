#include "dummyserver.h"

namespace FPLC
{

DummyServer::DummyServer(S7Cmd::CmdHandler* p):cmdHandler(p)
{

    if(nullptr == cmdHandler)
    {
        std::cout << "Error: No cmd handler." << std::endl;
        exit(1);
    }

    signal(SIGPIPE, [](int sig){ std::cout<<"\nsigpipe "<<sig<<std::endl;});

    signal(SIGTERM, [](int sig)
    {
        std::cout<<"\nsigterm " <<sig <<std::endl;
        exit(1);
    });


    signal(SIGINT, [](int sig)
    {
        std::cout<<"\n sigint " <<sig <<std::endl;
         exit(1);
    });


}


DummyServer::~DummyServer()
{
}



int DummyServer::run(uint16_t port)
{

    // simple one-thread solution for debug protocol

    //create listener
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(serv_addr.sin_zero), 0, 8);

    int listener = socket(AF_INET, SOCK_STREAM, 0); //setup server listener
    checkSock(listener);

    // to avoid error 'Address already in Use'
    int enableReuse = 1;

    if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(enableReuse)) < 0)
    {
        std::cout<< ("setsockopt failed");
        close(listener);
        exit(2);
    }

    //bind
    int bnd = bind(listener, (struct sockaddr *)&serv_addr, sizeof(serv_addr)); // bind listener
    checkBind(bnd);

    struct sockaddr client_addr;
    const int BACKLOG = 16;

    int lsn = listen(listener, BACKLOG); // start to listen connections
    int er = checkListen(lsn);

    //listen
    if(er < 0)
        std::cout<<"listener error"<<std::endl;

    struct epoll_event ev, events[16];
    int n, nfds, epollfd;

    epollfd = epoll_create1(0);

    if (epollfd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = listener;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listener, &ev) == -1)
    {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }

    socklen_t s_size = sizeof(struct sockaddr_in);

    for (;;)
    {
        nfds = epoll_wait(epollfd, events, 16, -1);

        if (nfds == -1)
        {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (n = 0; n < nfds; ++n)
        {
            if (events[n].data.fd == listener)
            {
                int client = accept(listener,
                                   (struct sockaddr *) &client_addr,
                                    &s_size);
                if (client == -1)
                {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                setnonblocking(client);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client;

                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client,
                              &ev) == -1)
                {
                    perror("epoll_ctl: conn_sock");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                //do job

                int client = events[n].data.fd;
                std::vector<uint8_t> buffer;
                buffer.resize(2*BUF_SIZE);
                ssize_t cnt = recv(client, &buffer[0], 2*BUF_SIZE, 0);

                if(-1 == cnt || 0 == cnt) //-1 == err, 0 == end of file, the remote has closed the connection
                {
                    close(client);
                    continue;
                }

                buffer.resize(cnt); //don't forget to resize it! or get rid of this expensive op...

                cmdHandler->processRequest(buffer, client);
            }
        }
    }
    close(listener);
}




int DummyServer::setnonblocking(int sockfd) // Setup nonblocking socket
{
    int flags;
#if defined(O_NONBLOCK)
   if(-1 == (flags = fcntl(sockfd, F_GETFL, 0)))
        flags = 0;
   return fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
#else
   flags = 1;
   return ioctl(sockfd, FIOBIO, &flags);
#endif
}


int DummyServer::checkSock(int sd)
{
    if(-1 == sd)
    {
        switch (errno)
        {
            case EACCES:
                std::cout<< "Socket() error: Permission to create a socket of the specified type and/or protocol is denied.";
                break;

            case EAFNOSUPPORT:
                std::cout<< "Socket() error: The implementation does not support the specified address family.";
                break;

            case EINVAL:
                std::cout<< "Socket() error: Unknown protocol, or protocol family not available.";
                break;

            case EMFILE:
                std::cout<< "Socket() error: Process file table overflow.";
                break;

            case ENFILE:
                std::cout<< "Socket() error: The system limit on the total number of open files has been reached.";
                break;

            case ENOMEM:
                std::cout<< "Socket() error: Insufficient memory is available. The socket cannot be created until sufficient resources are freed.";
                break;

            case EPROTONOSUPPORT:
                std::cout<< "Socket() error: The protocol type or the specified protocol is not supported within this domain";
                break;

            default:
                std::cout<< "Socket() error: Unknown socket() error.";
                break;
        }
    }
    return sd;
}


int DummyServer::checkBind(int ad)
{
    if(-1 == ad)
    {
        switch (errno)
        {
            case EACCES:
                std::cout<< "Bind error: The address is protected, and the user is not the superuser.";
                break;

            case EADDRINUSE:
                std::cout<< "Bind() error: The given address is already in use. ";
                break;

            case EBADF:
                std::cout<< "Bind() error: sockfd is not a valid descriptor.";
                break;

            case EINVAL:
                std::cout<< "Bind() error: The socket is already bound to an address.";
                break;

            case ENOTSOCK:
                std::cout<< "Bind() error: sockfd is a descriptor for a file, not a socket.";
                break;

            default:
                std::cout<< "Bind() error: Unknown bind() error.";
                break;
        }
    }
    return ad;
}


int DummyServer::checkListen(int ld)
{
    if(-1 == ld)
    {
        switch (errno)
        {
            case EADDRINUSE:
                std::cout<< "Listen() error: Another socket is already listening on the same port.";
                break;

            case EBADF:
                std::cout<< "Listen() error: The argument sockfd is not a valid descriptor." ;
                break;

            case ENOTSOCK:
                std::cout<< "Listen() error: The argument sockfd is not a socket.";
                break;

            case EOPNOTSUPP:
                std::cout<< "Listen() error: The socket is not of a type that supports the listen() operation.";
                break;

            default:
                std::cout<< "Error: Unknown listen() error.";
                break;
         }
    }
    return ld;
}


}//namespace
