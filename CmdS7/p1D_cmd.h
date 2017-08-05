#ifndef STARTUPLOADCMD_H
#define STARTUPLOADCMD_H

#include "command.h"


namespace S7Cmd
{
    /*
     * Description: start upload block to client
     */

    class StartUploadCmd: public Command
    {
        public:
            StartUploadCmd(FPLC::DbHandler & dbH);
            S7Status Execute(const std::vector<uint8_t> & input,
                             std::vector<uint8_t> & output,
                             SessionContext & ctx) override;



    };

} //namespace

#endif // STARTUPLOADCMD_H
