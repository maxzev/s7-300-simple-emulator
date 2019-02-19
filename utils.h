#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <stdarg.h>

/*
 *  Common utils.
 */

namespace FPLC
{

        /*****************************************************************************
        * std::string strArgs(int n, ...)
        * analogue of QString.arg(...).
        * Returns a copy of source string with a marker like '%1, %2 ...%99'
        * replaced by c-style strings 'a1, a2 ...a99' in one pass.
        *
        * First argument must be a number of args!!!
        * Second argument - source string.
        *
        * Example:
        *          string str =  "to %1 or %2 that is the %3";
        *          string result  = strArgs(5, &str[0], "be", "not to be", "question");
        *          // returns "to be or not to be that is the question";
        * if you want to use string in arguments:
        *          string s1 = "be";
        *          string s2 = "not to be";
        *          string s3 = "question";
        *          string result  = strArgs(5, &str[0], &s1[0], &s2[0], &s3[0]);
        *
        *****************************************************************************/

       std::string strArgs(int n, ...);


}// namespace

#endif // UTILS_H
