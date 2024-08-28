#include <iostream>

#include "boostUtils.h"

void printError(const beast::error_code& errorCode, const std::string& caller) {
    std::cerr << caller << ": " << errorCode.message() << '\n';
}
