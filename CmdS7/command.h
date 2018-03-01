#ifndef COMMAND_H
#define COMMAND_H

#include <stdint.h>
#include <vector>
#include <arpa/inet.h> // htons, ntohs
#include <cstring>     // memset, memcpy
#include <string>      // atoi

#include "../DBhandler/dbhandler.h"
#include "s7common.h"

namespace S7Cmd
{

    /*
     * WARNING! If you add a new command to S7CmdType,
     * don't forget to add it to GetCmdId(...)
     * and S7Commands!
     */

    enum class S7CmdType
    {
        None = 0xFFFF
    };


    /*
     * WARNING! If you add a new command to S7CmdType,
     * don't forget to add it to  GetCmdId(...)
     * and S7Commands!
     */

    uint16_t GetCmdId(S7CmdType c);


    /*
     * SesionContext class contains data for one session
     * between client and plc. Session is started with 0xF0 cmd
     * (start communication) and finished when plc or client closes connection.
     */
    struct SessionContext
    {
        int clientFd;
        uint16_t seqNum;
        uint16_t paramLength;
        uint16_t dataLength;
        BlockFd  blockFd;
        SzlFd    szlFd;

        SessionContext(): clientFd(-1),
                          seqNum(0),
                          paramLength(0),
                          dataLength(0)
        {
        }

        void clear()
        {
            // WARNING! Do not clear clientFd !
            seqNum = 0;
            paramLength = 0;
            dataLength = 0;

            blockFd.blockNum = 0;
            blockFd.blockType = 0;
            blockFd.uploadId = 0;
        }
    };
    typedef struct SessionContext SessionContext;



    /*
     * Base class for all commands.
     * Main responsibility of his descendants
     *  is to make an appropriate answer as real plc does.
     */

    class Command
    {
        public:
            Command(FPLC::DbHandler & dbH);
        
            /*
             * The NVI idiom should be used here, but it is ignored 
             * because Execute func is pure virtual and has only one impl in derived classes.
             */  
        
            virtual S7Status Execute(const std::vector<uint8_t> & input,
                                     std::vector<uint8_t> & output,
                                     SessionContext & ctx) = 0;

            virtual ~Command() {}
            uint32_t getId()   { return cmdId; }
            bool checkInput(const std::vector<uint8_t> & input,
                            size_t paramLength,
                            size_t dataLength,
                            size_t minBytes = 0); // minParamBytes - bytes of params+data
                                                  // that is necessary for certain cmd

        protected:
            uint32_t cmdId;
            size_t minPayload; //each cmd requires some bytes = params+data.

            FPLC::DbHandler & dbHandler;
    };



} //namespace

#endif // COMMAND_H
