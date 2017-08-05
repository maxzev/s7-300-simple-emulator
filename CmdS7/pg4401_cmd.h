#ifndef READSZLCMD_H
#define READSZLCMD_H

#include "command.h"
#include "../BlockS7/blk_s7.h"

namespace S7Cmd
{
    /*
     * Description: get certain SZL info.
     */

    class PG4401Cmd: public Command
    {
        public:
            PG4401Cmd(FPLC::DbHandler & dbH);
            S7Status Execute(const std::vector<uint8_t> & input,
                             std::vector<uint8_t> & output,
                             SessionContext & ctx) override;
         private:
            bool circle;
    };


}//namespace
#endif // READSZLCMD_H
