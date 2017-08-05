#include "pg4110_cmd.h"

namespace S7Cmd
{

    PG4110Cmd::PG4110Cmd(FPLC::DbHandler & dbH): Command(dbH)
    {
        cmdId = 0x4110;
        minPayload = 12+4; //12 bytes of params and 4 data
    }


    S7Status PG4110Cmd::Execute(const std::vector<uint8_t> & input,
                                std::vector<uint8_t> & output,
                                SessionContext & ctx)
    {
        if(!checkInput(input, ctx.paramLength, ctx.dataLength, minPayload))
            return S7Status::ErrInput;

        // read blockType
        // set pos to the begining of params area
        size_t p = sizeof(TPKT_Header)
                 + sizeof(DT_TPDU_Header)
                 + sizeof(S7COMM_Header10)
                 -1;

        uint8_t paramSeqNum = input[p + 8];

        //make answer

        //WARNING! hardcoded answer!
        //todo: parse input and make normal ans!
        std::array<uint8_t, 16> data = {0xFF, 0x09, 0x00, 0x0C,
                                        0x00, 0x04, 0x00, 0x04,
                                        0x01, 0x00, 0x00, 0x01,
                                        0x10, 0x02, 0x00, 0x00};


        S7COMM_Header10 s7Hdr10;
        s7Hdr10.rosctr  = 0x07;
        s7Hdr10.seqNum  = ctx.seqNum;
        s7Hdr10.parlg   = htons((uint16_t)sizeof(PgParams12));
        s7Hdr10.datlg   = htons((uint16_t)data.size());  // fake value, set further


        PgParams12 pgParams;
        pgParams.cmdCode    = 0x81;
        pgParams.subCmdCode = 0x10;
        pgParams.seqNum     = paramSeqNum;


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
        uint8_t* i = &output[0];

        memcpy(i, &tpkt, sizeof tpkt);
        i += sizeof tpkt;
        memcpy(i, &tpdu, sizeof tpdu);
        i += sizeof tpdu;
        memcpy(i, &s7Hdr10, sizeof s7Hdr10);
        i += sizeof s7Hdr10;
        memcpy(i, &pgParams, sizeof pgParams);
        i += sizeof pgParams;
        memcpy(i, &data[0], data.size());

        return S7Status::OK;
    }


}//namespace
