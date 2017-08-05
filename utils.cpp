#include "utils.h"

namespace FPLC
{

    std::string strArgs(int n, ...)
    {
        va_list vl;
        va_start(vl, n);
        std::string src = va_arg(vl, char*);
        size_t pos = 0;

        for(int i=0; i<n; ++i)
        {
            pos = src.find('%', pos);
            
            if(pos == std::string::npos || pos == src.length()-1)
                break;

            if(!isdigit(src[pos+1])) //filter separate '%' symbol without digit
            {
                ++pos;
                continue;
            }

            std::string temp;
            temp += src[pos];

            for(size_t next=pos+1; next<src.length(); ++next)
            {
                if(isdigit(src[next]))
                    temp += src[next];
                else
                    break;
            }

            std::string arg = va_arg(vl, char*);
            src.erase(pos, temp.length());
            src.insert(pos, arg);
        }

        va_end(vl);
        return src;
    }


}//namespace
