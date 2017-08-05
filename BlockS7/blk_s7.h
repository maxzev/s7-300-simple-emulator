#ifndef BLK_S7_H
#define BLK_S7_H

#include <stdint.h>
#include <vector>

/*
 * All stuff for blocks of s7_300
 */

namespace S7BLK
{


    struct S7BlkHeader
    {
        uint16_t  magic;        // 7070
        uint16_t  flags;
        uint8_t   lang;         // language of block: 01-STL, 02-LAD, 03-FBD, 05-DB, 07-SDB
        uint8_t   type;         // OB, DB, FC, FB, SDB, SFC, SFB
        uint16_t  number;       // block number
        uint32_t  blockLength;  // total length of block (header + body)
        uint32_t  blockSecurity;
        uint8_t   ts1[6];       // last modified
        uint8_t   ts2[6];       // last changed
        uint16_t  sub3Length;   // length of subblock 3
        uint16_t  sub2Length;   // length of subblock 2
        uint16_t  sub1Length;   // length of subblock 1
        uint16_t  sub0Length;   // length of subblock 0


        S7BlkHeader(): magic(0x7070)
        {
        }
    };
    typedef struct S7BlkHeader S7BlkHeader;


    enum class BlockType
    {
        OB  = 0x3038,    // 08 in ascii
        DB  = 0x3041,    // 0A in ascii
        SDB = 0x3042,    // 0B in ascii
        FC  = 0x3043,    // 0C in ascii
        SFC = 0x3044,    // 0D in ascii
        FB  = 0x3045,    // 0E in ascii
        SFB = 0x3046,    // 0F in ascii
        None = 0x0000
    };


    // special enum for SDB because we store it in a different tables
    enum class SDBtype
    {
        CP_SDB,
        CPU_SDB
    };

    uint16_t  GetBlockIdByType(BlockType t);
    BlockType GetBlockTypeById(uint16_t t);
    uint8_t   GetBlockLang(const std::vector<uint8_t> & rawBlk);




}//namespace

#endif // BLK_S7_H
