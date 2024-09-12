#include <fstream>
#include <chrono>
#include <random>

#include "ai/NeuralNetwork.h"
#include "ai/MCTS.h"
#include "ai/Example.h"

int main(int argc, char* argv[]) {
	NeuralNetwork neuralNetwork;
	std::ofstream lout("trainerLog.txt");
	if (argc >= 2) {
		if (!neuralNetwork.load(argv[1])) {
			lout << "ERROR: Starting current model did not load correctly from " << argv[1] << '\n';
		}
	} else {
		lout << "WARNING: No model was passed." << '\n';
	}
	
	std::ifstream fin("config.txt");
	
	if (fin.fail()) {
		lout << "FATAL: Config file did not load correctly" << '\n';
		return 1;
	}
	
	std::vector<int> config;
	int iTemp;
	for (int i = 0; i < 11; i++) {
		fin >> iTemp;
		
		if (fin.fail()) {
			lout << "FATAL: Config file did not load correctly" << '\n';
			return 1;
		}
		
		config.push_back(iTemp);
		fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	fin.close();
	
	const int NUM_ITERATIONS = config.at(0);
	const int NUM_EPISODES = config.at(1);
	const int NUM_SIMULATIONS = config.at(2);
	const int NUM_GAMES = config.at(3);
	const int EXPLORATION_TURNS = config.at(4);
	const int BATCH_SIZE = config.at(5);
	const int LOAD_EXAMPLES = config.at(6);
	const int SKIP_TRAINING = config.at(7);
	const int DISPLAY_GAMES = config.at(8);
	const int MAXIMUM_TURNS = config.at(9);
	const float RESULT_WEIGHT = config.at(10) / 100.0f;

	std::random_device seeder;
	auto rng = std::mt19937_64(seeder());
	std::uniform_real_distribution distribution(0.0, 1.0);
	MCTS mcts(&neuralNetwork, NUM_SIMULATIONS);
	for (int iteration = 0; iteration < NUM_ITERATIONS; iteration++) {
		lout << "Starting iteration " << iteration << '\n';
		lout.flush();
		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		
		std::vector<Example> examples;
		if (LOAD_EXAMPLES == 0 || iteration != 0) {
			lout << "Starting example generation" << '\n';
			lout << std::chrono::duration_cast<std::chrono::minutes>(std::chrono::steady_clock::now()-begin).count() << " minutes have passed" << '\n';
			lout.flush();

			std::ofstream exout("gameMCTSTemp.ex", std::ios::app);

			for (int episode = 0; episode < NUM_EPISODES; episode++) {
				GameState* curGameState = GameState::newGame('O', GameState::getRandomBoard(rng));
				
				int turns = 0;
				std::vector<float> probabilities;
				std::vector<std::tuple<std::vector<uint8_t>, std::vector<float>, float>> turnInformation;
				while (curGameState->getEndState() < -1) {
					probabilities = mcts.getMoveProbabilities(curGameState);
					
					turnInformation.emplace_back(toVector(curGameState), probabilities, mcts.getMoveValue(curGameState));

					int moveNum = 0;
					if (turns < EXPLORATION_TURNS) {
						float total = 0;
						auto target = static_cast<float>(distribution(rng));
						for (int i = 0; i < curGameState->getValidMoves()->size(); i++) {
							total += probabilities[curGameState->getValidMoves()->at(i) + 1];
							if (target < total) {
								moveNum = i;
								break;
							}
						}
					} else {
						float highestProbability = -1;
						int bestMove = -1;
						for (int i = 0; i < curGameState->getValidMoves()->size(); i++) {
							float probability = probabilities[curGameState->getValidMoves()->at(i) + 1];
							if (probability > highestProbability) {
								highestProbability = probability;
								bestMove = i;
							}
						}
						
						moveNum = bestMove;
					}
					
					GameState* child = curGameState->getChild(moveNum, false);
					delete curGameState;
					curGameState = child;
					mcts.reset();
					turns++;
				}
				
				float result = curGameState->getEndState();
				
				for (auto& turnInfo : turnInformation) {
					//Uses combination of neural network's evaluation and game result as target
					std::get<2>(turnInfo) = std::get<2>(turnInfo) * (1 - RESULT_WEIGHT) + result * RESULT_WEIGHT;
				}

				Example::save(exout, Example::load(turnInformation));
				exout.flush();

				delete curGameState;
			}

			exout.close();

			fin.open("gameMCTSTemp.ex");
			examples = Example::load(fin);
			fin.close();
		}
		
		if (SKIP_TRAINING == 0 || iteration != 0) {
			if (!neuralNetwork.save("models/temp.pt")) {
				lout << "ERROR: Current model did not save correctly to models/temp.pt" << '\n';
			}
			
			for (int epoch = 0; epoch < 10 * ((LOAD_EXAMPLES > 0 && iteration == 0) ? LOAD_EXAMPLES : 1); epoch++) {
				if (LOAD_EXAMPLES > 0 && iteration == 0) {
					std::uniform_int_distribution<int> exampleDistribution(1, LOAD_EXAMPLES);
					examples.clear();

					fin.open("examples/temp" + std::to_string(exampleDistribution(rng)) + ".ex");
					examples = Example::load(fin);
					fin.close();
				}
				
				lout << "Training with " << examples.size() << " examples." << '\n';
				lout << std::chrono::duration_cast<std::chrono::minutes>(std::chrono::steady_clock::now()-begin).count() << " minutes have passed" << '\n';
				lout.flush();
				
				neuralNetwork.train(examples, BATCH_SIZE);
				if (!neuralNetwork.save("models/temp2.pt")) {
					lout << "ERROR: Current model did not save correctly to models/temp2.pt" << '\n';
				}
			}
		}
		
		lout << "Iteration " << iteration << " took " << std::chrono::duration_cast<std::chrono::minutes>(std::chrono::steady_clock::now()-begin).count() << " minutes" << '\n';
		lout.flush();
	}
	
	return 0;
}