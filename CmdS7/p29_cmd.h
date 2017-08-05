#ifndef STOPPLC_CMD_H
#define STOPPLC_CMD_H

#include "command.h"

namespace S7Cmd
{

    /*
     * Description: stop plc befor uploading blocks
     */

    class StopPlcCmd: public Command
    {
        public:
            StopPlcCmd(FPLC::DbHandler & dbH);
            S7Status Execute(const std::vector<uint8_t> & input,
                             std::vector<uint8_t> & output,
                             SessionContext & ctx) override;
    };

}//namespace

#endif // STOPPLC_CMD_H
