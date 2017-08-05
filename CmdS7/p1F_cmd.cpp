#include "p1F_cmd.h"

namespace S7Cmd
{

    EndUploadCmd::EndUploadCmd(FPLC::DbHandler & dbH): Command(dbH)
    {
        cmdId = 0x001F;
        minPayload = 8;
    }

    S7Status EndUploadCmd::Execute(const std::vector<uint8_t> & input,
                                std::vector<uint8_t> & output,
                                SessionContext & ctx)
    {
        if(!checkInput(input, ctx.paramLength, ctx.dataLength, minPayload))
            return S7Status::ErrInput;

        size_t pos = sizeof(TPKT_Header)
                   + sizeof(DT_TPDU_Header)
                   + sizeof(S7COMM_Header10)
                   - 1; // set pos to the beginig of params

        // get channel
        uint16_t uploadId = (input[pos + 7] << 8) | input[pos + 8];

        // check uploadId
        if(uploadId != ctx.blockFd.uploadId)
        {
            ; // todo: handle error
        }

        S7COMM_Header12 s7Hdr12;
        s7Hdr12.rosctr  = 0x03;
        s7Hdr12.seqNum  = ctx.seqNum;
        s7Hdr12.parlg   = 0x0100;  // const, 1 in LE.
        s7Hdr12.datlg   = 0x0000;  // const, 0 in LE.
        s7Hdr12.errCode = 0x0000;

        uint8_t params[1];
        params[0] = 0x1f;          // func number end upload

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

        //clear context!
        ctx.clear();

        return S7Status::OK;
    }

}//namespace
