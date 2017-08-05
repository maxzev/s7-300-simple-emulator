#include "p1E_cmd.h"

namespace S7Cmd
{

    UploadDataCmd::UploadDataCmd(FPLC::DbHandler & dbH): Command(dbH)
    {
        cmdId = 0x001E;
        minPayload = 8;
    }


    S7Status UploadDataCmd::Execute(const std::vector<uint8_t> & input,
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

        // get UploadId
        uint16_t uploadId = (input[pos + 7] << 8) | input[pos + 8];

        // check uploadId
        if(uploadId != ctx.blockFd.uploadId)
            return S7Status::Err;

        //get block
        FPLC::MapIter bIter = dbHandler.getBlock(ctx.blockFd.blockType,
                                                 ctx.blockFd.blockNum);

        bool r = dbHandler.isBlockExists(bIter);

        if(!r)
            return S7Status::ErrNoBlock;

        uint16_t blockSize = (uint16_t)bIter->second.size();

        //make answer
        S7COMM_Header12 s7Hdr12;
        s7Hdr12.rosctr  = 0x03;
        s7Hdr12.seqNum  = ctx.seqNum;
        s7Hdr12.parlg   = 0x0200;               // const, 2 in LE.
        s7Hdr12.datlg   = htons(blockSize + 4); // 2 bytes => length,  2 bytes => 0x00FB
        s7Hdr12.errCode = 0x0000;

        uint8_t params[2];
        params[0] = 0x1E;  // func id
        params[1] = 0x00;  // 0x00 means all data in one packet

        const size_t DATA_SZ = 4;  // always 4 bytes
        std::vector<uint8_t> data; //todo: replace vec with array
        data.resize(DATA_SZ);
        data[0] = blockSize << 8;
        data[1] = blockSize;
        data[2] = 0x00;
        data[3] = 0xFB;

        // copy block. //todo: get rid of it!
        data.insert (data.begin() + DATA_SZ,
                     bIter->second.begin(),
                     bIter->second.end());

        DT_TPDU_Header tpdu;
        tpdu.length = 0x02;
        tpdu.type   = 0xF0;
        tpdu.number = 0x80;

        TPKT_Header tpkt;
        uint16_t totalLength = sizeof tpkt
                             + sizeof tpdu
                             + sizeof s7Hdr12
                             + sizeof params
                             + data.size();

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
        i += sizeof params;
        memcpy(i, &data[0], data.size());

        return S7Status::OK;
    }


}//namespace
