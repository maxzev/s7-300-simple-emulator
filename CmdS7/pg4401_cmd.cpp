#include "pg4401_cmd.h"

namespace S7Cmd
{

    PG4401Cmd::PG4401Cmd(FPLC::DbHandler & dbH): Command(dbH)
    {
        cmdId = 0x4401;
        minPayload = 8+6; //8 bytes of params and 6 data

        circle = false;
    }


    S7Status PG4401Cmd::Execute(const std::vector<uint8_t> & input,
                                std::vector<uint8_t> & output,
                                SessionContext & ctx)
    {
        if(!checkInput(input, ctx.paramLength, ctx.dataLength, minPayload))
            return S7Status::ErrInput;

        // set p to the begining of data
        size_t p = sizeof(TPKT_Header)
                 + sizeof(DT_TPDU_Header);


        uint8_t paramLength = ctx.paramLength;
        p += sizeof(S7COMM_Header10) + paramLength -1;


        // get  param length from params and end of packet
        // set pos to the begining of params
        size_t off = sizeof(TPKT_Header)
                   + sizeof(DT_TPDU_Header)
                   + sizeof(S7COMM_Header10)
                   -1;

        //uint8_t pl = input[off + 4]; //param length
        uint8_t paramSeqNum = input[off + 8];


        // fill s7 header

        S7COMM_Header10 s7Hdr10;
        s7Hdr10.rosctr  = 0x07;
        s7Hdr10.seqNum  = ctx.seqNum;
        s7Hdr10.parlg   = htons((uint16_t)sizeof(PgParams12));
        s7Hdr10.datlg   = 0; //fake value, set it further

        PgParams12 pgParams;
        pgParams.cmdCode    = 0x84;
        pgParams.subCmdCode = 0x01;
        pgParams.seqNum     = paramSeqNum;


        // get SZL_ID

        uint16_t szlId = (input[p+5] << 8) | input[p+6];
        uint16_t szlIdx = (input[p+7] << 8) | input[p+8];
        uint32_t idx = (szlId << 16) | szlIdx;

        std::vector<uint8_t> data;

        //get block
        std::map<uint32_t, std::vector<uint8_t> > & mr = dbHandler.SZL;
        auto it = mr.find(idx);

        if(it == mr.end())
        {
           //todo: logg error

            pgParams.errCode = 0x01d4;

            data.resize(4);
            data[0] = 0x0a;           // object doesn't exist
            data[1] = 0x00;
            data[2] = 0x00;           // length Hi
            data[3] = 0x00;           // length Lo
        }
        else
        {
            std::vector<uint8_t> & blk = it->second;
            data.resize(blk.size());
            uint8_t* pos = &data[0];
            memcpy(pos, &blk[0], blk.size());
        }


        DT_TPDU_Header tpdu;
        tpdu.length = 0x02;
        tpdu.type   = 0xF0;
        tpdu.number = 0x80;

        TPKT_Header tpkt;
        uint16_t totalLength = sizeof tpkt
                             + sizeof tpdu
                             + sizeof s7Hdr10
                             + sizeof pgParams
                             + data.size();

        tpkt.totalLength = htons(totalLength);

        output.resize(totalLength);
        uint8_t* pos = &output[0];

        //set data size to s7Header
        s7Hdr10.datlg   = htons((uint16_t)data.size());

        memcpy(pos, &tpkt, sizeof tpkt);
        pos += sizeof tpkt;
        memcpy(pos, &tpdu, sizeof tpdu);
        pos += sizeof tpdu;
        memcpy(pos, &s7Hdr10, sizeof s7Hdr10);
        pos += sizeof s7Hdr10;
        memcpy(pos, &pgParams, sizeof pgParams );
        pos += sizeof pgParams;
        memcpy(pos, &data[0], data.size());


        return S7Status::OK;
    }

}//namespace
