#include "cmdhandler.h"

namespace S7Cmd
{

    CmdHandler::CmdHandler(FPLC::DbHandler & dbHandler): s7Commands(nullptr)
    {
        s7Commands = new S7Commands(dbHandler);
    }


    CmdHandler::~CmdHandler()
    {
        delete s7Commands;
    }


    void CmdHandler::processRequest(const std::vector<uint8_t> &input, int clientFd)
    {
        //check input 1
        if(input.size() <= (sizeof(TPKT_Header) + 1)) // 1 == first field of TPDU header is tpdu-type
        {
            //todo: replace with logger
            return;
        }

        //handle COTP
        uint8_t CR_Pos = sizeof(TPKT_Header) + 1; // 1 == first field of TPDU header is tpdu-type
        uint8_t tpduType = input[CR_Pos];

        std::vector<uint8_t> output; // buffer for answer

        if(COTP_CR == tpduType)
        {
            bool r = sendCOTP_CR_Answer(input, clientFd);

            if(!r)
                ;//todo: replace with logger
        }
        else if(COTP_DT == tpduType)
        {
            //check input 2
            size_t off = sizeof(TPKT_Header) + sizeof(DT_TPDU_Header);

            if(input.size() < (off + sizeof(S7COMM_Header10) + 1)) //+1 => first byte of params
            {
                //no s7comm header in COTP packet
                //todo: replace with logger
                return;
            }

            const S7COMM_Header10* s7Hdr10 = (const S7COMM_Header10*)&input[off];

            if(0 == s7Hdr10->parlg)
            {
                //todo: replace with logger
                return;
            }

            // get id of function (first byte of params)
            size_t pos = off + sizeof(S7COMM_Header10);
            uint8_t pgNum = input[pos];

            //init SessionContext
            sessionCtx.clientFd    = clientFd;
            sessionCtx.seqNum      = s7Hdr10->seqNum;   // LE originally, don't need to convert
            sessionCtx.paramLength = ntohs(s7Hdr10->parlg);
            sessionCtx.dataLength  = ntohs(s7Hdr10->datlg);

            uint16_t cmdId = 0x0000;

            //handle case with PGRequest. Each subcommand is a separate cmd.
            if(0x00 == pgNum)
            {
                if(input.size() < (pos + sessionCtx.paramLength + sessionCtx.dataLength))
                {
                    //todo: replace with logger
                    std::cout<< "Err: input buffer is corrupted 3. " << " \n" <<std::endl;
                    return;
                }

                // read params
                pos = pos - 1;   //set pos to the begining of params area
                uint16_t cmdCode    = input[pos + 6];
                uint8_t subCmdCode  = input[pos + 7];

                cmdId = (cmdCode << 8) | subCmdCode;
            }
            else
            {
                cmdId = pgNum;
            }

            // process S7 command
            std::shared_ptr<Command> cmd = s7Commands->getCommand(cmdId);
            S7Status st = cmd->Execute(input, output, sessionCtx);

            if(S7Status::OK == st)
                sendAnswer(sessionCtx.clientFd, output);
            else
            {
                std::cout<< "Error occured in cmd: "<< cmdId << std::endl;

                if(S7Status::ErrInput == st)
                    std::cout<< "Wrong input.\n";
                else if(S7Status::ErrNoCmd == st)
                    std::cout<< "No such command.\n";
                else if(S7Status::ErrNoBlock == st)
                    std::cout<< "No such block.\n";

                std::cout<< std::endl;
            }
        } //else if
    }


    bool CmdHandler::sendCOTP_CR_Answer(const std::vector<uint8_t> & input, int clientFd)
    {
        //check input
        size_t sz = sizeof(TPKT_Header) + sizeof(CR_TPDU_Header);

        if(input.size() < sz)
        {
            //todo: replace with logger
            return false;
        }

        const CR_TPDU_Header* crHdr = (const CR_TPDU_Header*)&input[sizeof(TPKT_Header)];

        // make CC packet

        /*
         * All fields values below always the same for
         * DT = 0xE0, therefore just hardcoded here.
         */

        CC_TPDU_Header ccHdr;
        ccHdr.length = sizeof(CC_TPDU_Header) - 1; // todo: find out why -1 !!!
        ccHdr.type   = 0xD0;
        ccHdr.dstRef = crHdr->srcRef;
        ccHdr.srcRef = 0x3144;
        ccHdr.opts   = 0;

        ccHdr.srcTsapCode   = 0xC1;
        ccHdr.srcTsapLength = 0x02;
        ccHdr.srcTsapHi     = crHdr->srcTsapHi;
        ccHdr.srcTsapLo     = crHdr->srcTsapLo;

        ccHdr.dstTsapCode   = 0xC2;
        ccHdr.dstTsapLength = 0x02;
        ccHdr.dstTsapHi     = crHdr->dstTsapHi;
        ccHdr.dstTsapLo     = crHdr->dstTsapLo;

        ccHdr.tpduSizeCode   = 0xC0;
        ccHdr.tpduSizeLength = 0x01;
        ccHdr.tpduSize       = crHdr->tpduSize;


        TPKT_Header tpkt;
        tpkt.version  = 0x03;
        tpkt.reserved = 0x00;
        uint16_t totalPacketLength = sizeof(TPKT_Header) + sizeof(CC_TPDU_Header);
        tpkt.totalLength = htons(totalPacketLength);

        //output buff
        const size_t OUTPUT_SZ = sizeof(TPKT_Header) + sizeof(CC_TPDU_Header);
        uint8_t output [OUTPUT_SZ];
        uint8_t* pos = output;

        memcpy(pos, &tpkt, sizeof tpkt);
        pos += sizeof tpkt;
        memcpy(pos, &ccHdr, sizeof ccHdr);

        //send COTP answer
        ptrdiff_t sendedBytes = sendAll(clientFd, (const char*)output, OUTPUT_SZ);

        return ((ptrdiff_t)OUTPUT_SZ == sendedBytes);
    }


    ptrdiff_t CmdHandler::sendAnswer(int client, const std::vector<uint8_t> & buffer)
    {
        size_t sz = buffer.size();
        ptrdiff_t total = sendAll(client, (const char*)&buffer[0], sz);
        return total;
    }


    ptrdiff_t CmdHandler::sendAll(int fd, const char *buf, size_t len)
    {
        size_t total = 0;
        size_t bytes_left = len;
        ptrdiff_t n = 0;

        while(total < len)
        {
            n = send(fd, buf+total, bytes_left, 0); // replace with sendmmsg!! send many packeta in one time!
            if(n == -1)
                break;
            total += n;
            bytes_left -= n;
        }

        return n == -1? n: total; // return 0 - success
    }

} // namespace
