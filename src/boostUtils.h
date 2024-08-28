#ifndef BOOST_UTILS_H
#define BOOST_UTILS_H

#include <boost/beast/core.hpp>

namespace beast = boost::beast;

/**
 * @brief Prints error message
 * @param errorCode error code
 * @param caller where the error occurred
 */
void printError(const beast::error_code& errorCode, const std::string& caller);

#endif