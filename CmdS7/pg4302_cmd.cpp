#include "pg4302_cmd.h"

namespace S7Cmd
{

    PG4302Cmd::PG4302Cmd(FPLC::DbHandler & dbH): Command(dbH)
    {
        cmdId = 0x4302;
        minPayload = 8+6; //8 bytes of params and 6 data
    }


    S7Status PG4302Cmd::Execute(const std::vector<uint8_t> & input,
                                std::vector<uint8_t> & output,
                                SessionContext & ctx)
    {
        if(!checkInput(input, ctx.paramLength, ctx.dataLength, minPayload))
        {
            std::cout << input.size() << " "
                      << ctx.paramLength << " "
                      << ctx.dataLength << " \n";
            return S7Status::ErrInput;
        }

        // read blockType
        // set pos to the begining of params area
        size_t p = sizeof(TPKT_Header)
                 + sizeof(DT_TPDU_Header)
                 + sizeof(S7COMM_Header10)
                 -1;

        uint8_t paramSeqNum = input[p + 8];

        uint16_t tmp = input[p + 13];
        uint16_t blockType = (tmp << 8) + (input[p + 14]);


        // make answer
        FPLC::MapRef mr = dbHandler.getBlockListByType((S7BLK::BlockType)blockType);
        std::vector<uint8_t> data; //todo: optimize it!
        uint16_t paramErrCode = 0x0000;

        if(!mr.empty())
        {
            //fill first 4 bytes before items
            uint16_t sz = mr.size();
            sz *= 4;                  // size of all items: 4 -byte struct for each entry
            data.reserve(sz + 4);     // size of all items + 4 bytes data info

            data.push_back(0xFF);     // ret code, 0xFF - success
            data.push_back(0x09);     // transport size. I don't know why 0x09

            uint8_t t = sz >> 8;
            data.push_back(t);        // size Hi
            t = sz;
            data.push_back(t);        // size Lo

            // make items
            auto itb = mr.begin();

            while(itb != mr.end())
            {
                uint8_t n = (itb->first) >> 8;
                data.push_back(n);    // block number Hi
                n = itb->first;
                data.push_back(n);    // block number Lo
                data.push_back(0x22); // flags. for SFC/SDB == 0x42, OB/DB/FC/FB/SDB == 0x12 or 0x22
                data.push_back( S7BLK::GetBlockLang(itb->second) ); // block lang

                ++itb;
            }
        }
        else
        {
            paramErrCode = 0xD20E;    // param error: No block availiable
            data.push_back(0x0A);     // data error: Object doesn't exist
            data.push_back(0x00);     // transport size
            data.push_back(0x00);
            data.push_back(0x00);
        }


        DT_TPDU_Header tpdu;
        tpdu.length = 0x02;
        tpdu.type   = 0xF0;
        tpdu.number = 0x80;
        
        
        S7COMM_Header10 s7Hdr10;
        s7Hdr10.rosctr = 0x07;
        s7Hdr10.seqNum = ctx.seqNum;
        s7Hdr10.parlg  = htons((uint16_t)sizeof(PgParams12));
        s7Hdr10.datlg   = htons((uint16_t)data.size());


        PgParams12 pgParams;
        pgParams.cmdCode    = 0x83;
        pgParams.subCmdCode = 0x02;
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


} //namespace
