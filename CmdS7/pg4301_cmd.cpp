#include "pg4301_cmd.h"

namespace S7Cmd
{

    PG4301Cmd::PG4301Cmd(FPLC::DbHandler & dbH): Command(dbH)
    {
        cmdId = 0x4301;
        minPayload = 8+4; //8 bytes of params and 6 data
    }


    S7Status PG4301Cmd::Execute(const std::vector<uint8_t> & input,
                                std::vector<uint8_t> & output,
                                SessionContext & ctx)
    {
        if(!checkInput(input, ctx.paramLength, ctx.dataLength, minPayload))
            return S7Status::ErrInput;

        // read blockType
        //set pos to the begining of params area
        size_t p = sizeof(TPKT_Header)
                 + sizeof(DT_TPDU_Header)
                 + sizeof(S7COMM_Header10)
                 -1;

        uint8_t paramSeqNum = input[p + 8];


        // make answer
        auto itB = dbHandler.blockCaches.begin();
        auto itE = dbHandler.blockCaches.end();

        std::vector<uint8_t> data; //todo: optimize it!
        data.reserve(4);
        //fill first 4 bytes before items
        data.push_back(0xFF);     // ret code, 0xFF - success
        data.push_back(0x09);     // transport size. I don't know why 0x09
        data.push_back(0x00);     // fake Hi size, will be filled further
        data.push_back(0x00);     // fake Lo size, will be filled further

        uint16_t paramErrCode = 0x0000;

        for(; itB != itE; ++itB)
        {
            FPLC::MapRef mr = dbHandler.getBlockListByType(*itB);

            if(!mr.empty())
            {
                // make items
                uint16_t blkType  = S7BLK::GetBlockIdByType(*itB);
                uint16_t blkCount = (uint16_t)mr.size();

                uint8_t t = blkType >> 8;
                data.push_back(t);
                t = blkType;
                data.push_back(t);

                t = blkCount >> 8;
                data.push_back(t);
                t = blkCount;
                data.push_back(t);
            }
            else
            {
                //todo: handle err

//                paramErrCode = 0xD20E;    // param error: No block availiable
//                data.push_back(0x0A);     // data error: Object doesn't exist
//                data.push_back(0x00);     // transport size
//                data.push_back(0x00);
//                data.push_back(0x00);
            }
        }//for

        //size of data
        uint16_t sz = (uint16_t)data.size() - 4; //size of items only!
        uint8_t t = sz >> 8;
        data[2] = t;        // size Hi insted of fake
        t = sz;
        data[3] = t;        // size Lo insted of fake

        DT_TPDU_Header tpdu;
        tpdu.length = 0x02;
        tpdu.type   = 0xF0;
        tpdu.number = 0x80;


        S7COMM_Header10 s7Hdr10;
        s7Hdr10.rosctr  = 0x07;
        s7Hdr10.seqNum  = ctx.seqNum;
        s7Hdr10.parlg   = htons((uint16_t)sizeof(PgParams12));
        s7Hdr10.datlg   = htons((uint16_t)data.size());


        PgParams12 pgParams;
        pgParams.cmdCode    = 0x83;
        pgParams.subCmdCode = 0x01;
        pgParams.seqNum     = paramSeqNum;
        pgParams.errCode    = htons(paramErrCode);


        TPKT_Header tpkt;
        uint16_t totalLength = sizeof tpkt
                             + sizeof tpdu
                             + sizeof s7Hdr10
                             + sizeof pgParams
                             + data.size();

        tpkt.totalLength = htons(totalLength);

        output.resize(totalLength);
        uint8_t* pos = &output[0];

        memcpy(pos, &tpkt, sizeof tpkt);
        pos += sizeof tpkt;
        memcpy(pos, &tpdu, sizeof tpdu);
        pos += sizeof tpdu;
        memcpy(pos, &s7Hdr10, sizeof s7Hdr10);
        pos += sizeof s7Hdr10;
        memcpy(pos, &pgParams, sizeof pgParams);
        pos += sizeof pgParams;
        memcpy(pos, &data[0], data.size());

        return S7Status::OK;

    }

}//namespace
