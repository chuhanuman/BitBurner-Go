#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

/**
 * @brief Splits a string by a given delimiter
 * @param str string to split
 * @param delimiter delimiter to split string by
 * @param allowEmpty whether empty strings should be kept
 * @return vector with all the parts of the string split by the given delimiter
 */
std::vector<std::string> split(const std::string& str, const char delimiter, bool allowEmpty = false);

#endif