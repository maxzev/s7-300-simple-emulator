#ifndef ENDUPLOAD_H
#define ENDUPLOAD_H


#include "command.h"


namespace S7Cmd
{
    /*
     * Description: is sended after uploading block to client
     */

    /*
     *  WARNING!!!
     *  This command must clear context at the end.
     */

    class EndUploadCmd: public Command
    {
        public:
            EndUploadCmd(FPLC::DbHandler & dbH);
            S7Status Execute(const std::vector<uint8_t> & input,
                             std::vector<uint8_t> & output,
                             SessionContext & ctx) override;
    };

} //namespace

#endif // ENDUPLOAD_H
