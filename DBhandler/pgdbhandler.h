#ifndef DBHANDLER_H
#define DBHANDLER_H


#include <arpa/inet.h> // htons, ntohs

#include "postgresql/libpq-fe.h"

#include "dbhandler.h"
#include "utils.h"

namespace FPLC
{

    class PgDbHandler: public DbHandler
    {
        public:

            PgDbHandler();
            ~PgDbHandler();
            bool loadBlockListByType(S7BLK::BlockType type);
            
            PgDbHandler(const PgDbHandler & rhs) = delete;
            PgDbHandler & operator=(const PgDbHandler & rhs) = delete;
            
        private:
            PGconn *conn;
            bool isConnection;

            //todo: temporary solution! fix it
            bool loadSDBBlocks(S7BLK::SDBtype type);
    };




} //namespace

#endif // DBHANDLER_H
