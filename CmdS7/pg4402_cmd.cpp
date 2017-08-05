#include "pg4402_cmd.h"

namespace S7Cmd
{

    PG4402Cmd::PG4402Cmd(FPLC::DbHandler & dbH): Command(dbH)
    {
        cmdId = 0x4402;
        minPayload = 8+4; //8 bytes of params and 4 data
    }


    S7Status PG4402Cmd::Execute(const std::vector<uint8_t> & input,
                                 std::vector<uint8_t> & output,
                                 SessionContext & ctx)
    {
        if(!checkInput(input, ctx.paramLength, ctx.dataLength, minPayload))
            return S7Status::ErrInput;

        //set pos to the begining of params area
        size_t p = sizeof(TPKT_Header)
                 + sizeof(DT_TPDU_Header)
                 + sizeof(S7COMM_Header10)
                -1;

        uint8_t paramSeqNum = input[p + 8];

        uint16_t subscribe = input[p + 5];
        subscribe |= input[p + 6];

        //make answer
        const uint16_t DATA_LENGTH = 0x0006; // 6 bytes, const for this cmd.

        S7COMM_Header10 s7Hdr10;
        s7Hdr10.rosctr  = 0x07;
        s7Hdr10.seqNum  = ctx.seqNum;
        s7Hdr10.parlg   = htons((uint16_t)sizeof(PgParams12));
        s7Hdr10.datlg   = htons(DATA_LENGTH);


        PgParams12 pgParams;
        pgParams.cmdCode    = 0x84;
        pgParams.subCmdCode = 0x02;
        pgParams.seqNum     = paramSeqNum;


        uint8_t data[DATA_LENGTH];
        data[0] = 0xff;
        data[1] = 0x09;
        data[2] = 0x00;           // length Hi
        data[3] = 0x02;           // length Lo
        data[4] = 0x02;           // result     todo: make enum Result!
        data[5] = 0x00;

        DT_TPDU_Header tpdu;
        tpdu.length = 0x02;
        tpdu.type   = 0xF0;
        tpdu.number = 0x80;

        TPKT_Header tpkt;
        uint16_t totalLength = sizeof tpkt
                             + sizeof tpdu
                             + sizeof s7Hdr10
                             + sizeof pgParams
                             + sizeof data;

        tpkt.totalLength = htons(totalLength);

        output.resize(totalLength);
        uint8_t* pos = &output[0];

        memcpy(pos, &tpkt, sizeof tpkt);
        pos += sizeof tpkt;
        memcpy(pos, &tpdu, sizeof tpdu);
        pos += sizeof tpdu;
        memcpy(pos, &s7Hdr10, sizeof s7Hdr10);
        pos += sizeof s7Hdr10;
        memcpy(pos, &pgParams, sizeof pgParams );
        pos += sizeof pgParams;
        memcpy(pos, data, (size_t)DATA_LENGTH);

        return S7Status::OK;
    }




}//namespace
