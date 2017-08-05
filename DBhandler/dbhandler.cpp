#include "dbhandler.h"

namespace FPLC
{

    DbHandler::DbHandler()
    {
       
        /*
         * Read SZL blocks from directory
         */

    }


    DbHandler::~DbHandler()
    {
    }


    bool DbHandler::isCacheEmpty(S7BLK::BlockType type)
    {
        if(S7BLK::BlockType::OB == type)
            return OB.begin() == OB.end();
        else if(S7BLK::BlockType::DB == type)
            return DB.begin() == DB.end();
        else if(S7BLK::BlockType::SDB == type)
            return SDB.begin() == SDB.end();
        else if(S7BLK::BlockType::FB == type)
            return FB.begin() == FB.end();
        else if(S7BLK::BlockType::FC == type)
            return FC.begin() == FC.end();
        else
            return false;
    }



    bool DbHandler::isBlockExists(MapIter it)
    {
        return it != EMPTY.begin();
    }



    MapRef DbHandler::getBlockListByType(S7BLK::BlockType type)
    {
        if(S7BLK::BlockType::OB == type)
            return OB;
        else if(S7BLK::BlockType::DB == type)
            return DB;
        else if(S7BLK::BlockType::SDB == type)
            return SDB;
        else if(S7BLK::BlockType::FB == type)
            return FB;
        else if(S7BLK::BlockType::FC == type)
            return FC;

        return EMPTY;
    }


    //obsolete func for reading from file
    void DbHandler::readBlock(std::string path, std::vector<uint8_t> & buffer)
    {
        std::ifstream file (path, std::ios::binary);

        if(!file)
            return;

        std::streampos begin = file.tellg();
        file.seekg (0, std::ios::end);
        std::streampos end = file.tellg();
        file.seekg (0, file.beg);

        int size = end - begin;

        if(0 == size)
            return ;

        buffer.resize(size);
        char* buff = (char*)&buffer[0];
        file.read(buff, size);

        file.close();
    }


    uint16_t DbHandler::getBlockLength(uint16_t blockNum, uint16_t blockType)
    {
        bool r = isCacheEmpty(S7BLK::GetBlockTypeById(blockType));

        if(r)
            return 0;

        MapRef mr = getBlockListByType(S7BLK::GetBlockTypeById(blockType));
        auto it = mr.find(blockNum);

        if(it == mr.end())
            return 0;

        return (uint16_t)it->second.size();
    }


    MapIter DbHandler::getBlock(uint16_t blockType, uint16_t blockNum)
    {
        MapRef mr = getBlockListByType(S7BLK::GetBlockTypeById(blockType));

        if(EMPTY == mr)
            return EMPTY.begin();

        auto it = mr.find(blockNum);

        if(it == mr.end())
            return EMPTY.begin();

        if(it->second.empty())
            return EMPTY.begin();

        return it;
    }

} //namespace
