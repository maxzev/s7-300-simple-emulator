#include <iostream>
#include <vector>

#include "dummyserver.h"
#include "DBhandler/pgdbhandler.h"
#include "CmdS7/cmdhandler.h"



int main()
{
    FPLC::PgDbHandler dbhandler;
    S7Cmd::CmdHandler* cmdHandler = new S7Cmd::CmdHandler(dbhandler);

    FPLC::DummyServer server(cmdHandler);
    server.run(102);   //real plc use port 102 !

    delete cmdHandler;

    return 0;
}
