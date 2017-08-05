#ifndef PG4110_CMD_H
#define PG4110_CMD_H


#include "command.h"
#include "../BlockS7/blk_s7.h"


namespace S7Cmd
{
    /*
     * Description: get list of debug tasks.
     * Depends on szl_0x0132_0002.
     */

    class PG4110Cmd: public Command
    {
        public:
            PG4110Cmd(FPLC::DbHandler & dbH);
            S7Status Execute(const std::vector<uint8_t> & input,
                             std::vector<uint8_t> & output,
                             SessionContext & ctx) override;
    };


}//namespace

#endif // PG4110_CMD_H
