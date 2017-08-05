#include "blk_s7.h"

namespace S7BLK
{
    uint16_t GetBlockIdByType(BlockType t)
    {
        switch(t)
        {
            case BlockType::OB    : return 0x3038;  break;
            case BlockType::DB    : return 0x3041;  break;
            case BlockType::SDB   : return 0x3042;  break;
            case BlockType::FC    : return 0x3043;  break;
            case BlockType::SFC   : return 0x3044;  break;
            case BlockType::FB    : return 0x3045;  break;
            case BlockType::SFB   : return 0x3046;  break;
            case BlockType::None  : return 0x0000;  break;

            default               : return 0x0000;  break;
        }
    }


    BlockType GetBlockTypeById(uint16_t t)
    {
        switch(t)
        {
            case 0x3038  : return BlockType::OB;   break;
            case 0x3041  : return BlockType::DB;   break;
            case 0x3042  : return BlockType::SDB;  break;
            case 0x3043  : return BlockType::FC;   break;
            case 0x3044  : return BlockType::SFC;  break;
            case 0x3045  : return BlockType::FB;   break;
            case 0x3046  : return BlockType::SFB;  break;
            case 0x0000  : return BlockType::None; break;

            default      : return BlockType::None; break;
        }
    }


    uint8_t GetBlockLang(const std::vector<uint8_t> & rawBlk)
    {
        if(sizeof(S7BlkHeader) >= rawBlk.size())
            return 0;

        S7BlkHeader* hdr = (S7BlkHeader*)&rawBlk[0];
        uint8_t lang = hdr->lang;

        return lang;
    }



}//namespace
