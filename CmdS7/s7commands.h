#ifndef S7COMMANDS_H
#define S7COMMANDS_H

#include <map>
#include <memory>

#include "../DBhandler/dbhandler.h"

#include "command.h"
#include "none_cmd.h"
#include "pF0_cmd.h"
#include "p1D_cmd.h"
#include "p1E_cmd.h"
#include "p1F_cmd.h"
#include "p29_cmd.h"

#include "pg4401_cmd.h"
#include "pg4402_cmd.h"
#include "pg4301_cmd.h"
#include "pg4302_cmd.h"
#include "pg4303_cmd.h"
#include "pg4110_cmd.h"




namespace S7Cmd
{
    typedef std::map<uint16_t, std::shared_ptr<Command> >::iterator ICmd;

    /*
     * Class container for S7 commands.
     *
     */
    class S7Commands
    {
        public:
            S7Commands(FPLC::DbHandler & dbH);
            std::shared_ptr<Command> getCommand(uint16_t cmdId);

        private:
            FPLC::DbHandler & dbHandler;
            std::map<uint16_t, std::shared_ptr<Command> > s7commands;

            void addCommand(std::shared_ptr<Command> cmd);
    };


}//namespace

#endif // S7COMMANDS_H
