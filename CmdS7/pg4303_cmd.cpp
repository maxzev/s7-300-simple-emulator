#include "pg4303_cmd.h"


namespace S7Cmd
{

    PG4303Cmd::PG4303Cmd(FPLC::DbHandler & dbH): Command(dbH)
    {
        cmdId = 0x4303;
        minPayload = 8+12; //8 bytes of params and 12 data
    }

    S7Status PG4303Cmd::Execute(const std::vector<uint8_t> & input,
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
        p += ctx.paramLength; // set p to the first byte of data

        // get block type that we need to send back
        uint16_t blockType = (input[p + 5] << 8) | (input[p + 6]);

        char blockNumStr [5]; //this crap is ascii representation of block number!!!
        blockNumStr[0] = input[p + 8];
        blockNumStr[1] = input[p + 9];
        blockNumStr[2] = input[p + 10];
        blockNumStr[3] = input[p + 11];
        blockNumStr[4] = '\0';

        uint16_t blockNum = std::atoi(blockNumStr);
        //uint8_t filesystem = input[pos + 12];

        //find a block
        if(dbHandler.isCacheEmpty(S7BLK::GetBlockTypeById(blockType)) )
        {
            //todo: handle err
             //std::cout<< __LINE__ << __FILE__ <<std::endl;
        }


        FPLC::MapRef mr = dbHandler.getBlockListByType(S7BLK::GetBlockTypeById(blockType));

        if(mr.empty())
        {
           //todo: handle err and send answer with err
        }
        else
        {
            FPLC::MapIter iter = dbHandler.getBlock(blockType, blockNum);

            if(!dbHandler.isBlockExists(iter))
            {
                //todo: handle err
            }

            //make answer
            PG4303DataPreHeader preHdr4303;
            const uint16_t SUB3 = 36; // actually it is fixed size of subblock3 and footer
            const size_t dataSize = sizeof(S7BLK::S7BlkHeader) + SUB3 + sizeof preHdr4303;

            uint8_t data[dataSize];
            memcpy(data, &preHdr4303, sizeof preHdr4303);

            //copy header
            std::copy(iter->second.begin(),
                      iter->second.begin() + sizeof(S7BLK::S7BlkHeader),
                      data + sizeof preHdr4303);


            //copy subblock3
            std::copy(iter->second.end() - SUB3, // size of subblock3 + footer
                      iter->second.end(),
                      data + sizeof preHdr4303 + sizeof(S7BLK::S7BlkHeader));


            //fill s7 header
            S7COMM_Header10 s7Hdr10;
            s7Hdr10.rosctr  = 0x07;
            s7Hdr10.seqNum  = ctx.seqNum;
            s7Hdr10.parlg   = htons((uint16_t)sizeof(PgParams12));
            s7Hdr10.datlg   = htons(dataSize);  // fake value, set further


            PgParams12 pgParams;
            pgParams.cmdCode    = 0x83;
            pgParams.subCmdCode = 0x03;
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
                                 + dataSize;

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
            memcpy(i, data, dataSize);

            return S7Status::OK;

        }//else

        return S7Status::ErrNoBlock;
    }


}//namespace
