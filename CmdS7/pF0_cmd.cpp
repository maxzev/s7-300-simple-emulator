#include "pF0_cmd.h"

namespace S7Cmd
{

    SetupCommCmd::SetupCommCmd(FPLC::DbHandler & dbH): Command(dbH)

    {
        cmdId = 0x00F0;
        minPayload = 8; // 8 bytes of params
    }

    S7Status SetupCommCmd::Execute(const std::vector<uint8_t> & input,
                                   std::vector<uint8_t> & output,
                                   SessionContext & ctx)
    {
        if(!checkInput(input, ctx.paramLength, ctx.dataLength, minPayload))
            return S7Status::ErrInput;

        // make answer
        TPKT_Header tpkt;
        uint16_t totalPacketLength = sizeof(TPKT_Header)
                                   + sizeof(DT_TPDU_Header)
                                   + sizeof(S7COMM_Header12)
                                   + sizeof(S7COMM_F0_params);

        tpkt.totalLength = htons(totalPacketLength);

        /*
         * All fields values further always the same for
         * DT = 0xF0, therefore just hardcoded here.
         */

        DT_TPDU_Header dtTpdu;
        dtTpdu.length = 0x02;
        dtTpdu.type   = 0xF0;
        dtTpdu.number = 0x80;

        S7COMM_Header12 s7Hdr12;
        s7Hdr12.rosctr  = 0x03;
        s7Hdr12.seqNum  = ctx.seqNum;
        s7Hdr12.parlg   = 0x0800; // const, 0x0008 in LE
        s7Hdr12.datlg   = 0x0000;
        s7Hdr12.errCode = 0x0000;

        S7COMM_F0_params F0_Params;

        output.resize(totalPacketLength);
        uint8_t* pos = &output[0];

        memcpy(pos, &tpkt, sizeof tpkt);
        pos += sizeof tpkt;
        memcpy(pos, &dtTpdu, sizeof dtTpdu);
        pos += sizeof dtTpdu;
        memcpy(pos, &s7Hdr12, sizeof s7Hdr12);
        pos += sizeof s7Hdr12;
        memcpy(pos, &F0_Params, sizeof F0_Params);

        return S7Status::OK;
    }


} //namespace
