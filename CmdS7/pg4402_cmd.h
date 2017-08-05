#ifndef PG4402_CMD_H
#define PG4402_CMD_H


#include "command.h"
#include "../BlockS7/blk_s7.h"

#include <iostream>

namespace S7Cmd
{

    /*
     * Description: message service command.
     */

    class PG4402Cmd: public Command
    {
        public:
            PG4402Cmd(FPLC::DbHandler & dbH);
            S7Status Execute(const std::vector<uint8_t> & input,
                             std::vector<uint8_t> & output,
                             SessionContext & ctx) override;
    };



} //namespace
#endif // PG4402_CMD_H
