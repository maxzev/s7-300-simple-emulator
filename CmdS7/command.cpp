#include "command.h"

namespace S7Cmd
{

    Command::Command(FPLC::DbHandler & dbH): cmdId((uint32_t)S7CmdType::None),
                                             minPayload(0),
                                             dbHandler(dbH)
    {
    }


    bool Command::checkInput(const std::vector<uint8_t> & input,
                             size_t paramLength,
                             size_t dataLength,
                             size_t minBytes)
    {
        // each cmd requires a certain param and data
        // do this check to drop query with strange param and data length
        if(0 != minBytes)
        {
            bool noParams = (paramLength + dataLength) < minBytes;

            if(noParams)
                return false;
        }

        size_t sz = sizeof(TPKT_Header)
                  + sizeof(DT_TPDU_Header)
                  + sizeof(S7COMM_Header10)
                  + paramLength
                  + dataLength;

        return input.size() >= sz;
    }


    uint16_t GetCmdId(S7CmdType c)
    {
        switch(c)
        {
            case S7CmdType::None        : return 0xFFFF; break;

            //a bit obsolete now
//            case S7CmdType::PG4110      : return 0x4110; break;
//            case S7CmdType::PG4401      : return 0x4401; break;
//            case S7CmdType::PG4402      : return 0x4402; break;
//            case S7CmdType::PG4301      : return 0x4301; break;
//            case S7CmdType::PG4302      : return 0x4302; break;
//            case S7CmdType::PG4303      : return 0x4303; break;
//            case S7CmdType::SetupComm   : return 0x00F0; break;
//            case S7CmdType::StartUpload : return 0x001D; break;
//            case S7CmdType::UploadData  : return 0x001E; break;
//            case S7CmdType::EndUpload   : return 0x001F; break;
//            case S7CmdType::StopPlc     : return 0x0029; break;

            default                     : return 0xFFFF; break;
        }
    }

} //namespace
