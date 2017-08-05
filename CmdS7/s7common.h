#ifndef MISC_H
#define MISC_H

#include <stdint.h>
#include <string>
#include <algorithm>

namespace S7Cmd
{


    // TPKT Header - ISO on TCP - RFC 1006 (4 bytes)
    struct TPKT_Header
    {
        uint8_t version;      // Always 3 for RFC 1006
        uint8_t reserved;     // always 0x00
        uint16_t totalLength; // length of packet (entire frame, payload and TPDU included)

        TPKT_Header(): version(0x03),
                       reserved(0x00),
                       totalLength(0)
        {
        }
    };
    typedef struct TPKT_Header TPKT_Header;


    typedef struct
    {
        uint8_t  length;
        uint8_t  type;             //0xE0
        uint16_t dstRef;
        uint16_t srcRef;
        uint8_t  opts;
        //3 params
        uint8_t  srcTsapCode;      // param1 code: 0xC1 == src-tsap
        uint8_t  srcTsapLength;    // param1 length: 0x02
        uint8_t  srcTsapHi;        // param1 value, 1 byte
        uint8_t  srcTsapLo;        // param1 value, 2 byte
        uint8_t  dstTsapCode;      // param2 code: 0xC2 == dst-tsap
        uint8_t  dstTsapLength;    // param2 length: 0x02
        uint8_t  dstTsapHi;        // param2 value, 1 byte
        uint8_t  dstTsapLo;        // param2 value, 2 byte
        uint8_t  tpduSizeCode;     // param3 code: 0xC0 == tpdu-size.
        uint8_t  tpduSizeLength;   // param3 length: 0x01
        uint8_t  tpduSize;         // param3 value: 9 == 512, 0x0A == 1024 bytes.

    }CR_TPDU_Header;               // connection request


    typedef struct
    {
        uint8_t  length;
        uint8_t  type;             // == 0xD0
        uint16_t dstRef;
        uint16_t srcRef;
        uint8_t  opts;
        //3 params
        uint8_t  tpduSizeCode;     // param1 code: 0xC0 == tpdu-size. 9 == 512, 0x0A == 1024 bytes.
        uint8_t  tpduSizeLength;   // param1 length: 0x01
        uint8_t  tpduSize;         // param1 value
        uint8_t  srcTsapCode;      // param2 code: 0xC1 == src-tsap
        uint8_t  srcTsapLength;    // param2 length: 0x02
        uint8_t  srcTsapHi;        // param2 value, 1 byte
        uint8_t  srcTsapLo;        // param2 value, 2 byte
        uint8_t  dstTsapCode;      // param3 code: 0xC2 == dst-tsap
        uint8_t  dstTsapLength;    // param3 length: 0x02
        uint8_t  dstTsapHi;        // param3 value, 1 byte
        uint8_t  dstTsapLo;        // param3 value, 2 byte

    }CC_TPDU_Header;               // connection confirmation


    typedef struct
    {
        uint8_t  length;
        uint8_t  type;             // == 0xF0
        uint8_t number;            // if == 0x80 than Last Data Unit
    }DT_TPDU_Header;               // data transmission


    typedef struct
    {
        uint8_t  length;
        uint8_t  type;             // == 0x80
        uint16_t dstRef;
        uint16_t srcRef;
        uint8_t  reason;
    }DR_TPDU_Header;               // reset connection




    //S7
    //header of 10 bytes for rosctr == 1 or 7
    struct S7COMM_Header10
    {
        uint8_t  protid;   // always 0x32
        uint8_t  rosctr;
        uint16_t reserved; // always 0x00
        uint16_t seqNum;   // sequence number
        uint16_t parlg;    // param length
        uint16_t datlg;    // data length

        S7COMM_Header10(): protid(0x32),
                           reserved(0x00)
        {
        }
    };
    typedef struct S7COMM_Header10 S7COMM_Header10;



    //header of 12 bytes for rosctr == 3, ack_data
    struct S7COMM_Header12
    {
        uint8_t  protid;   // always 0x32
        uint8_t  rosctr;   // Remote Operating Service Control: 1 -request, 3 -ack, 7 -data
        uint16_t reserved; // always 0x00
        uint16_t seqNum;   // sequence number
        uint16_t parlg;    // param length
        uint16_t datlg;    // data length
        uint16_t errCode;

        S7COMM_Header12(): protid(0x32),
                           reserved(0x00)
        {
        }

    };
    typedef struct S7COMM_Header12 S7COMM_Header12;


    struct S7COMM_F0_params
    {
        uint8_t  pgNum;          // number of function
        uint8_t  reserved;       // always 0x00
        uint16_t maxAqm1calling; // parallel jobs with ack
        uint16_t maxAqm1called;  // parallel jobs with ack
        uint16_t pduLength;      // PDU length

        S7COMM_F0_params(): pgNum(0xF0),
                            reserved(0x00),
                            maxAqm1calling(0x0100), // always == 1 (in LE) for this type of pgNum
                            maxAqm1called(0x0100),  // always == 1 (in LE) for this type of pgNum
                            pduLength(0xF000)       // always == 0xF0 (in LE) for s7_300
        {
        }
    };
    typedef struct S7COMM_F0_params S7COMM_F0_params;


    enum class S7Status
    {
        OK,
        Err,
        ErrInput,    // something wrong with input buffer
        ErrNoCmd,    // no such cmd
        ErrNoBlock   // no such block
    };


    typedef struct
    {
        uint8_t  uploadId;
        uint16_t blockType;
        uint16_t blockNum;
    }BlockFd;


    struct SzlFd
    {
        uint32_t prevSzlId;
        bool     need2Part;

        SzlFd(): prevSzlId(0xffff),
                 need2Part(false)
        {
        }

        void clear()
        {
            prevSzlId = 0xffff;
            need2Part = false;
        }

    };
    typedef struct SzlFd SzlFd;


    //for pg4303
    struct PG4303DataPreHeader
    {                          // values below looks like constants for pg4303:
        uint8_t  resCode;      //0xFF success code
        uint8_t  transpSize;   //0x09
        uint16_t blockLength;  //0x4e00 == 78 in LE
        uint16_t blockType;    //0x0001 == 1 in LE
        uint16_t infoLenght;   //0x4a00 == 74 in LE
        uint16_t blockInfo;    //0x0022 == 0x2200 in LE

        PG4303DataPreHeader():  resCode(0xFF),
                                transpSize(0x09),
                                blockLength(0x4e00),
                                blockType(0x0001),
                                infoLenght(0x4a00),
                                blockInfo(0x0022)
        {
        }

    };
    typedef struct PG4303DataPreHeader PG4303DataPreHeader;


    //pg params. Mostly used parameters set in PG commands.
    struct PgParams12
    {                        // values below looks like constants for pg4303:
        uint8_t  pgCode;     //0x00
        uint8_t  head1;      //0x01
        uint8_t  head2;      //0x12
        uint8_t  pLength;    //0x08
        uint8_t  pLenc;      //0x12 ,encoded length of params 0x11 == 8, 0x12 == 12 bytes
        uint8_t  cmdCode;
        uint8_t  subCmdCode;
        uint8_t  seqNum;     // don't mix it with seqNum in S7Header!
        uint8_t  dataUnit;   //0x00
        uint8_t  lastUnit;   //0x00 == last unit, 0x01 ==  there are more data
        uint16_t errCode;

        PgParams12(): pgCode(0x00),
                    head1(0x01),
                    head2(0x12),
                    pLength(0x08),
                    pLenc(0x12),
                    cmdCode(0x00),
                    subCmdCode(0x00),
                    seqNum(0x00),
                    dataUnit(0x00),
                    lastUnit(0x00),
                    errCode(0)
        {
        }

    };
    typedef struct PgParams12 PgParams12;

    static const uint8_t COTP_DT = 0xF0; // COTP type DT
    static const uint8_t COTP_CR = 0xE0; // COTP type CR

    void uint16_to_5_byte_string( uint16_t value, char* out);

} //namespace

#endif // MISC_H
