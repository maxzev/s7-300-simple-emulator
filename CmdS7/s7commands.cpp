#include "s7commands.h"

namespace S7Cmd
{

    S7Commands::S7Commands(FPLC::DbHandler & dbH): dbHandler(dbH)
    {
        //create commands
        try
        {
            addCommand(std::shared_ptr<NoneCmd>(new NoneCmd(dbHandler)));
            addCommand(std::shared_ptr<SetupCommCmd>(new SetupCommCmd(dbHandler)));
            addCommand(std::shared_ptr<StartUploadCmd>(new StartUploadCmd(dbHandler)));
            addCommand(std::shared_ptr<UploadDataCmd>(new UploadDataCmd(dbHandler)));
            addCommand(std::shared_ptr<EndUploadCmd>(new EndUploadCmd(dbHandler)));
            addCommand(std::shared_ptr<StopPlcCmd>(new StopPlcCmd(dbHandler)));

            addCommand(std::shared_ptr<PG4110Cmd>(new PG4110Cmd(dbHandler)));
            addCommand(std::shared_ptr<PG4301Cmd>(new PG4301Cmd(dbHandler)));
            addCommand(std::shared_ptr<PG4302Cmd>(new PG4302Cmd(dbHandler)));
            addCommand(std::shared_ptr<PG4303Cmd>(new PG4303Cmd(dbHandler)));
            addCommand(std::shared_ptr<PG4401Cmd>(new PG4401Cmd(dbHandler)));
            addCommand(std::shared_ptr<PG4402Cmd>(new PG4402Cmd(dbHandler)));

        }
        catch(...)
        {
//            //todo: replace with logger!
//            std::cout<<"Can't create commands. Exit \n";
//            exit(1);
        }
    }


    void S7Commands::addCommand(std::shared_ptr<Command> cmd)
    {
        s7commands[cmd->getId()] = cmd;
    }


    std::shared_ptr<Command> S7Commands::getCommand(uint16_t cmdId)
    {
        auto it = s7commands.find(cmdId);

        if (s7commands.end() == it)
        {
            auto i = s7commands.find(GetCmdId(S7CmdType::None));

            if(s7commands.end() == i)
                throw std::runtime_error("Critical Error: no cmd None \n");

            return i->second;
        }

        return it->second;

    }


}//namespace
