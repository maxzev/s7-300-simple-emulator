#ifndef PG4302Cmd_H
#define PG4302Cmd_H


#include "command.h"
#include "../BlockS7/blk_s7.h"


namespace S7Cmd
{

    /*
     * Description: get list of blocks of certain type.
     */

    class PG4302Cmd: public Command
    {
        public:
            PG4302Cmd(FPLC::DbHandler & dbH);
            S7Status Execute(const std::vector<uint8_t> & input,
                             std::vector<uint8_t> & output,
                             SessionContext & ctx) override;
    };

} //namespace

#endif // PG4302Cmd_H
