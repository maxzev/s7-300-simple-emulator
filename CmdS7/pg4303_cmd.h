#ifndef PPG4303_CMD_H
#define PPG4303_CMD_H

#include <array>

#include "command.h"
#include "../BlockS7/blk_s7.h"


namespace S7Cmd
{
    /*
     * Description: get block info.
     * Command returns a header of certain block and subblock 3.
     */



    class PG4303Cmd: public Command
    {
        public:
            PG4303Cmd(FPLC::DbHandler & dbH);
            S7Status Execute(const std::vector<uint8_t> & input,
                             std::vector<uint8_t> & output,
                             SessionContext & ctx) override;
    };

}//namespace

#endif // PPG4303_CMD_H
