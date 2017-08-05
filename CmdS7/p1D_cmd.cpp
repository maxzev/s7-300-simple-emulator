#include "p1D_cmd.h"

namespace S7Cmd
{

    StartUploadCmd::StartUploadCmd(FPLC::DbHandler & dbH): Command(dbH)

    {
        cmdId = 0x001D;
        minPayload = 18;
    }

    S7Status StartUploadCmd::Execute(const std::vector<uint8_t> & input,
                                     std::vector<uint8_t> & output,
                                     SessionContext & ctx)
    {
        if(!checkInput(input, ctx.paramLength, ctx.dataLength, minPayload))
            return S7Status::ErrInput;

        // set pos to the begining of params
        size_t pos = sizeof(TPKT_Header)
                   + sizeof(DT_TPDU_Header)
                   + sizeof(S7COMM_Header10)
                   -1;

        // get block type that we need to send back
        uint16_t blockType = (input[pos + 11] << 8) | (input[pos + 12]);

        char blockNumStr [5]; //this crap is ascii representation of block number!!!
        blockNumStr[0] = input[pos + 14];
        blockNumStr[1] = input[pos + 15];
        blockNumStr[2] = input[pos + 16];
        blockNumStr[3] = input[pos + 17];
        blockNumStr[4] = '\0';

        uint16_t blockNum = std::atoi(blockNumStr);
        uint16_t blockLength = dbHandler.getBlockLength(blockNum, blockType);


        //make answer
        S7COMM_Header12 s7Hdr12;
        uint8_t params[16];

        if(0 != blockLength)
        {
            s7Hdr12.rosctr  = 0x03;
            s7Hdr12.seqNum  = ctx.seqNum;
            s7Hdr12.parlg   = 0x1000;  // const val. 16 in LE.
            s7Hdr12.datlg   = 0x0000;  // 0 in LE.
            s7Hdr12.errCode = 0x0000;

            params[0] = 0x1D;  // func number
            params[1] = 0x00;  // func status
            params[2] = 0x01;  // send data
            params[3] = 0x00;
            params[4] = 0x00;
            params[5] = 0x00;

            //init ctx.blockFd for next cmd
            ctx.blockFd.blockType = blockType;
            ctx.blockFd.blockNum  = blockNum;
            ctx.blockFd.uploadId  = 0x07; // always 0x07 for that plc we had on tests

            //upload ID, value is chosen for each block that client requests.
            params[6] = 0x00;
            params[7] = ctx.blockFd.uploadId;

            // block length string (rest of bytes further)
            params[8] = 0x07;

            //length of block in ascii
            char tmp[6] = "00000";
            uint16_to_5_byte_string(blockLength, tmp);

            params[9] = 0x30;
            params[10] = 0x30;
            params[11] = tmp[0];
            params[12] = tmp[1];
            params[13] = tmp[2];
            params[14] = tmp[3];
            params[15] = tmp[4];
        }
        else
        {
           // todo: handle errors

            s7Hdr12.rosctr  = 0x03;
            s7Hdr12.seqNum  = ctx.seqNum;
            s7Hdr12.parlg   = 0x1000;  // 16 in LE.
            s7Hdr12.datlg   = 0x0000;  // 0 in LE.
            s7Hdr12.errCode = 0x0010;  // todo:  set appropriate error(0x0010 is not correct err!)!
        }


        DT_TPDU_Header tpdu;
        tpdu.length = 0x02;
        tpdu.type   = 0xF0;
        tpdu.number = 0x80;

        TPKT_Header tpkt;
        uint16_t totalLength = sizeof tpkt
                             + sizeof tpdu
                             + sizeof s7Hdr12
                             + sizeof params;

        tpkt.totalLength = htons(totalLength);

        output.resize(totalLength);
        uint8_t* i = &output[0];

        memcpy(i, &tpkt, sizeof tpkt);
        i += sizeof tpkt;
        memcpy(i, &tpdu, sizeof tpdu);
        i += sizeof tpdu;
        memcpy(i, &s7Hdr12, sizeof s7Hdr12);
        i += sizeof s7Hdr12;
        memcpy(i, params, sizeof params);

        return S7Status::OK;
    }


}//namespace
