#ifndef UPLOADDATA_H
#define UPLOADDATA_H


#include "command.h"


namespace S7Cmd
{
    /*
     * Description: sends block to client
     */

    class UploadDataCmd: public Command
    {
        public:
            UploadDataCmd(FPLC::DbHandler & dbH);
            S7Status Execute(const std::vector<uint8_t> & input,
                             std::vector<uint8_t> & output,
                             SessionContext & ctx) override;
    };

} //namespace

#endif // UPLOADDATA_H
