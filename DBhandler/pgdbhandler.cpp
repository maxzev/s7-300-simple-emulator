#include "pgdbhandler.h"

namespace FPLC
{

    PgDbHandler::PgDbHandler(): DbHandler()
    {
        /*
         * Connect to database and load blocks to cache.
         */
    }


    PgDbHandler::~PgDbHandler()
    {
        if(conn)
            PQfinish(conn);
    }



} //namespace
