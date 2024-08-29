#include <string>
#include <vector>

#include "utils.h"

std::vector<std::string> split(const std::string& str, const char delimiter, const bool allowEmpty) {
	std::vector<std::string> parts;
	
	std::string curPart;
	for (const char c : str) {
		if (c == delimiter) {
			if (!curPart.empty() || allowEmpty) {
				parts.push_back(curPart);
				curPart.clear();
			}
		} else {
			curPart.push_back(c);
		}
	}
	if (!curPart.empty()) {
		parts.push_back(curPart);
	}
	
	return parts;
}
