#include "none_cmd.h"

namespace S7Cmd
{

    NoneCmd::NoneCmd(FPLC::DbHandler & dbH): Command(dbH)
    {
        cmdId = 0xFFFF;
    }

    S7Status NoneCmd::Execute(const std::vector<uint8_t> & input,
                                   std::vector<uint8_t> & output,
                                   SessionContext & ctx)
    {
        //todo: replace with logger
        //std::cout<< "Error: wrong command. \n";

        return S7Status::ErrNoCmd;
    }

}//namespace
