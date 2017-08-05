#ifndef CMDHANDLER_H
#define CMDHANDLER_H

#include <sys/socket.h>
#include <cstddef> // ptrdiff_t
#include "s7commands.h"

namespace S7Cmd
{
    /*
     * CmdHandler makes COTP answer and calls correspondent cmd of S7Comm.
     */

    class CmdHandler
    {
        typedef std::map<uint16_t, std::shared_ptr<Command> >::iterator ICmd;

        public:
            CmdHandler(FPLC::DbHandler & dbHandler);
            ~CmdHandler();
            void processRequest(const std::vector<uint8_t> & input, int clientFd);

        private:
            S7Commands* s7Commands;
            SessionContext sessionCtx; // one sessionCtx object for single-client mode.

            //todo: think how to split the logic better
            ptrdiff_t sendAnswer(int client, const std::vector<uint8_t> & buffer);
            ptrdiff_t sendAll(int fd, const char *buf, size_t len);

            bool sendCOTP_CR_Answer(const std::vector<uint8_t> & input, int clientFd);
    };


} // namespace

#endif // CMDHANDLER_H
