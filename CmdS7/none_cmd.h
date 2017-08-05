#ifndef NONECMD_H
#define NONECMD_H

#include "s7commands.h"

namespace S7Cmd
{

    class NoneCmd: public Command
    {
    public:
        NoneCmd(FPLC::DbHandler & dbH);
        S7Status Execute(const std::vector<uint8_t> & input,
                         std::vector<uint8_t> & output,
                        SessionContext & ctx) override;
    };

} //namespace

#endif // NONECMD_H
