#include <fstream>

#include "ai/Example.h"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout << "Pass in a file please." << '\n';
		return -1;
	}

	std::ifstream fin(argv[1]);
	const std::vector<Example> examples = Example::load(fin);

	for (auto& example : examples) {
		example.display(std::cout);
		std::cout << '\n';
	}

	return 0;
}
