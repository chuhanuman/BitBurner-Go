#include "Example.h"

#include <unordered_map>

#include "GameStateData.h"
#include "../utils.h"

std::vector<Example> Example::load(const std::vector<std::tuple<std::vector<uint8_t>, std::vector<float>, float>>& turnInformation) {
	std::vector<Example> examples;
	examples.reserve(turnInformation.size());
	for (auto& turn : turnInformation) {
		Example example;
		example.gameStateData = std::get<0>(turn);
		example.moveProbabilities = std::get<1>(turn);
		example.value = std::get<2>(turn);

		examples.push_back(example);
	}

	return examples;
}

std::vector<Example> Example::load(const std::vector<std::pair<std::vector<uint8_t>, std::vector<float>>>& turnInformation, const float value) {
	std::vector<Example> examples;
	examples.reserve(turnInformation.size());
	for (auto& [gameStateData, moveProbabilities] : turnInformation) {
		Example example;
		example.gameStateData = gameStateData;
		example.moveProbabilities = moveProbabilities;
		example.value = value;
		
		examples.push_back(example);
	}
	
	return examples;
}

std::vector<Example> Example::load(std::istream& in) {
	std::vector<Example> examples;
	
	while (!(in.eof() || in.fail())) {
		Example example;
		std::string line;
		
		std::getline(in, line);
		
		if (in.fail()) {
			break;
		} else if (line.empty()) {
			continue;
		}
		
		std::vector<std::string> elements = split(line, ' ');
		for (unsigned int i = 0; i < GAME_STATE_DATA_LENGTH; i++) {
			example.gameStateData.push_back(elements.at(0).at(i));
		}
		for (unsigned int i = 1; i < elements.size() - 1; i++) {
			example.moveProbabilities.push_back(std::stof(elements.at(i)));
		}
		example.value = std::stof(elements.at(elements.size() - 1));

		examples.push_back(example);
	}
	
	return examples;
}

bool Example::safeLoad(std::istream& in, std::vector<Example>& examples) {
	constexpr int MAX_EXAMPLES = 65536;
	int count = 0;
	while (!(in.eof() || in.fail()) && count < MAX_EXAMPLES) {
		Example example;
		std::string line;
		
		std::getline(in, line);
		
		if (in.fail()) {
			break;
		} else if (line.empty()) {
			continue;
		}
		
		std::vector<std::string> elements = split(line, ' ');
		for (unsigned int i = 0; i < GAME_STATE_DATA_LENGTH; i++) {
			example.gameStateData.push_back(elements.at(0).at(i));
		}
		for (unsigned int i = 1; i < elements.size() - 1; i++) {
			example.moveProbabilities.push_back(std::stof(elements.at(i)));
		}
		example.value = std::stof(elements.at(elements.size() - 1));

		examples.push_back(example);
		count++;
	}
	
	return (in.eof() || in.fail());
}

void Example::save(std::ostream& out, const std::vector<Example>& examples) {
	out << std::fixed;
	for (const Example& example : examples) {
		for (const uint8_t num : example.gameStateData) {
			out << num;
		}
		out << ' ';
		for (const float moveProbability : example.moveProbabilities) {
			//Checks if float can be printed as an int
			if (moveProbability == ceilf(moveProbability)) {
				out << static_cast<int>(moveProbability) << ' ';
			} else {
				out << moveProbability << ' ';
			}
		}
		
		//Checks if float can be printed as an int
		if (example.value == ceilf(example.value)) {
			out << static_cast<int>(example.value);
		} else {
			out << example.value;
		}
		out << '\n';
	}
	out.flush();
	out << std::defaultfloat;
}

void Example::saveAverage(std::istream& in, std::ostream& out) {
	std::unordered_map<std::string, std::vector<std::pair<std::vector<float>, float>>> examples;
	while (!(in.eof() || in.fail())) {
		std::string state;
		std::pair<std::vector<float>, float> result;
		std::string line;
		
		std::getline(in, line);
		
		if (in.fail()) {
			break;
		} else if (line.empty()) {
			continue;
		}
		
		std::vector<std::string> elements = split(line, ' ');
		for (unsigned int i = 0; i < GAME_STATE_DATA_LENGTH; i++) {
			state.push_back(elements.at(0).at(i));
		}
		for (unsigned int i = 1; i < elements.size() - 1; i++) {
			result.first.emplace_back(std::stof(elements.at(i)));
		}
		result.second = std::stof(elements.at(elements.size() - 1));

		if (examples.find(state) == examples.end()) {
			std::vector<std::pair<std::vector<float>, float>> temp;
			examples.emplace(state, temp);
		}

		examples.at(state).push_back(result);
	}

	out << std::fixed;
	for (const auto& [state, results] : examples) {
		for (const uint8_t num : state) {
			out << num;
		}
		out << ' ';

		std::vector<float> totalProbabilities;
		totalProbabilities.resize(NUM_MOVES, 0);
		float totalValue = 0;
		for (const auto& [moveProbabilities, value] : results) {
			for (size_t move = 0; move < moveProbabilities.size(); move++) {
				totalProbabilities[move] += moveProbabilities[move];
			}
			totalValue += value;
		}

		for (float moveProbability : totalProbabilities) {
			moveProbability /= results.size();
			//Checks if float can be printed as an int
			if (moveProbability == ceilf(moveProbability)) {
				out << static_cast<int>(moveProbability) << ' ';
			} else {
				out << moveProbability << ' ';
			}
		}

		//Checks if float can be printed as an int
		const float value = totalValue / results.size();
		if (value == ceilf(value)) {
			out << static_cast<int>(value);
		} else {
			out << value;
		}
		out << '\n';
	}
	out.flush();
	out << std::defaultfloat;
}

std::vector<uint8_t>& Example::getGameStateData() {
	return gameStateData;
}

std::vector<float> Example::getMoveProbabilities() {
	return moveProbabilities;
}

float Example::getValue() const {
	return value;
}

void Example::display(std::ostream& out) const {
	GameState* gameState = getGameState(gameStateData);
	out << *gameState;

	for (const int move : *gameState->getValidMoves()) {
		out << move << " " << moveProbabilities.at(move + 1) << '\n';
	}

	out << value << '\n';

	delete gameState;
}
