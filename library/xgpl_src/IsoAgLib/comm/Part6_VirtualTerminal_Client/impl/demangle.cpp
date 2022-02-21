/*
 * demangle.cpp
 *
 *  Created on: 21.02.2022
 *      Author: franz
 */

#include <string>
#include <cxxabi.h>
#include "demangle.h"



using std::string;

/* Helper function which converts typeid().name() to a human-readable type name */
string demangle(const char* name)
{
    int status = 0;
    char* result = abi::__cxa_demangle(name, NULL, NULL, &status);
    string str_result;
    if (status == 0) {
        str_result = result;
    } else {
        str_result = name;
    }
    free(result);
    return str_result;
}
