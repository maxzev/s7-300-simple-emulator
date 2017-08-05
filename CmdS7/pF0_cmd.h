#ifndef SETUPCOMMCMD_H
#define SETUPCOMMCMD_H

#include "command.h"

namespace S7Cmd
{
    /*
     * Description: setup communication with plc
     */


    class SetupCommCmd: public Command
    {
        public:
            SetupCommCmd(FPLC::DbHandler & dbH);
            S7Status Execute(const std::vector<uint8_t> & input,
                             std::vector<uint8_t> & output,
                             SessionContext & ctx) override;

    };


} //namespace

#endif // SETUPCOMMCMD_H
