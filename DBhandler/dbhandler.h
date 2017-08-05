#ifndef DbHandler_H
#define DbHandler_H

#include <map>
#include <fstream>
#include <array>


#include "settings.h"
#include "BlockS7/blk_s7.h"

namespace FPLC
{

    /*
     * DbHandler contains a cache for blocks of different types.
     * This class doesn't know how to and wherefrom
     * load data to cache. This routine is implemented by his descendants.
     */

    typedef std::map<uint16_t, std::vector<uint8_t> > & MapRef;
    typedef std::map<uint16_t, std::vector<uint8_t> >::iterator MapIter;
    typedef std::map<uint16_t, std::vector<uint8_t> >* MapPtr;


    class DbHandler
    {
        public:
            DbHandler();
            ~DbHandler();
            MapRef getBlockListByType(S7BLK::BlockType type);
            bool isCacheEmpty(S7BLK::BlockType type);
            bool isBlockExists(MapIter it);
            void readBlock(std::string path, std::vector<uint8_t> & buffer);
            uint16_t getBlockLength(uint16_t blockNum, uint16_t blockType);
            MapIter getBlock(uint16_t blockType, uint16_t blockNum);


            //for demonstration only!!! Don't take it close to your heart)

            std::map<uint32_t, std::vector<uint8_t> > SZL;
            std::vector<uint8_t> szl_132_4_1;
            std::vector<uint8_t> szl_132_4_2;

            /*
             * That code further until protected section is an auxiliary stuff
             * to make code more clean in PG4301Cmd.
             * It supposed to be a temporary solution and will be removed soon.
             *
             */
            // auxiliary array of type names
            // total amount of caches now: OB, DB, FB, FC, SDB, SFC, SFB
            std::array<S7BLK::BlockType, 7> blockCaches;

        protected:
            // simple local cache for blocks
            std::map<uint16_t, std::vector<uint8_t> > OB;
            std::map<uint16_t, std::vector<uint8_t> > DB;
            std::map<uint16_t, std::vector<uint8_t> > FB;
            std::map<uint16_t, std::vector<uint8_t> > FC;
            std::map<uint16_t, std::vector<uint8_t> > SDB;
            std::map<uint16_t, std::vector<uint8_t> > SFC;
            std::map<uint16_t, std::vector<uint8_t> > SFB;

            std::map<uint16_t, std::vector<uint8_t> > EMPTY; //this map is always empty
    };




} //namespace

#endif // DbHandler_H
