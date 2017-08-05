#ifndef PG4301_CMD_H
#define PG4301_CMD_H

#include "command.h"
#include "../BlockS7/blk_s7.h"


namespace S7Cmd
{
    /*
     * Description: get list of blocks of all types.
     */


    class PG4301Cmd: public Command
    {
        public:
            PG4301Cmd(FPLC::DbHandler & dbH);
            S7Status Execute(const std::vector<uint8_t> & input,
                             std::vector<uint8_t> & output,
                             SessionContext & ctx) override;
    };

}//namespace

#endif // PG4301_CMD_H
