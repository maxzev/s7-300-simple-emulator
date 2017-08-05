#include "p29_cmd.h"

namespace S7Cmd
{

    StopPlcCmd::StopPlcCmd(FPLC::DbHandler & dbH): Command(dbH)

    {
        cmdId = 0x0029;
        minPayload = 16; // params only
    }

    S7Status StopPlcCmd::Execute(const std::vector<uint8_t> & input,
                                 std::vector<uint8_t> & output,
                                 SessionContext & ctx)
    {
        if(!checkInput(input, ctx.paramLength, ctx.dataLength, minPayload))
            return S7Status::ErrInput;


        S7COMM_Header12 s7Hdr12;
        s7Hdr12.rosctr  = 0x03;
        s7Hdr12.seqNum  = ctx.seqNum;
        s7Hdr12.parlg   = 0x01;      // const value
        s7Hdr12.datlg   = 0x00;      // const value
        s7Hdr12.errCode = 0x0000;

        // one byte of params
        uint8_t param = 0x29;

        DT_TPDU_Header tpdu;
        tpdu.length = 0x02;
        tpdu.type   = 0xF0;
        tpdu.number = 0x80;


        TPKT_Header tpkt;
        uint16_t totalLength = sizeof tpkt
                             + sizeof tpdu
                             + sizeof s7Hdr12
                             + 1;  // 1 byte of params

        tpkt.totalLength = htons(totalLength);

        output.resize(totalLength);
        uint8_t* i = &output[0];

        memcpy(i, &tpkt, sizeof tpkt);
        i += sizeof tpkt;
        memcpy(i, &tpdu, sizeof tpdu);
        i += sizeof tpdu;
        memcpy(i, &s7Hdr12, sizeof s7Hdr12);
        i += sizeof s7Hdr12;
        memcpy(i, &param, sizeof param);


        return S7Status::OK;
    }




}//namespace
