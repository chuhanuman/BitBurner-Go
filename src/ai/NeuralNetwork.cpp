#include <torch/torch.h>

#include <algorithm>

#include "NeuralNetwork.h"

#include "GameStateData.h"

std::pair<std::vector<float>, float> NeuralNetwork::predict(const GameState* gameState) {
	Device device = torch::cuda::is_available() ? torch::Device(kCUDA) : Device(kCPU);
	
	NoGradGuard noGrad;
	net.eval();
	net.to(device);

	std::vector<uint8_t> gameStateData = toVector(gameState);

	const Tensor tGameState = torch::from_blob(gameStateData.data(), {GAME_STATE_DATA_SIZE[0], GAME_STATE_DATA_SIZE[1], GAME_STATE_DATA_SIZE[2]}, dtype(kBool)).clone().to(device);
	
	std::vector<Tensor> results = net.forward(tGameState);
	results.at(0) = results.at(0).exp();
	const Tensor tProbabilities = results.at(0).cpu();
	std::vector<float> probabilities(tProbabilities.data_ptr<float>(), tProbabilities.data_ptr<float>() + tProbabilities.numel());

	return {probabilities, results.at(1).item<float>()};
}

void NeuralNetwork::train(std::vector<Example>& examples, const int batchSize) {
	torch::Device device = torch::cuda::is_available() ? torch::Device(torch::kCUDA) : torch::Device(torch::kCPU);
	torch::optim::Adam optimizer(net.parameters());
	net.train();
	net.to(device);

	std::shuffle(examples.begin(), examples.end(), rng);
	
	const int batchCount = static_cast<int>(examples.size()) / batchSize;
	for (int batchNum = 0; batchNum < batchCount; batchNum++) {
		std::vector<float> games;
		std::vector<float> probabilities;
		std::vector<float> values;
		
		for (int i = 0; i < batchSize; i++) {
			const int index = batchNum * batchSize + i;
			
			games.insert(games.end(), examples.at(index).getGameStateData().begin(), examples.at(index).getGameStateData().end());
			std::vector<float> moveProbabilities = examples.at(index).getMoveProbabilities();
			probabilities.insert(probabilities.end(), moveProbabilities.begin(), moveProbabilities.end());
			values.push_back(examples.at(index).getValue());
		}
		
		const Tensor tGames = torch::from_blob(games.data(), {batchSize, GAME_STATE_DATA_SIZE[0], GAME_STATE_DATA_SIZE[1], GAME_STATE_DATA_SIZE[2]}).clone().to(device);
		Tensor tProbabilities = torch::from_blob(probabilities.data(), {batchSize, NUM_MOVES}).clone().to(device);
		Tensor tValues = torch::from_blob(values.data(), {batchSize, 1}).clone().to(device);
		
		std::vector<Tensor> results = net.forward(tGames);
		
		Tensor probabilitiesLoss = -1 * sum(tProbabilities.multiply(results.at(0))) / batchSize;
		Tensor valuesLoss = sum(torch::pow(tValues - results.at(1).view(-1), 2)) / batchSize;
		Tensor totalLoss = probabilitiesLoss + valuesLoss;
		
		optimizer.zero_grad();
		totalLoss.backward();
		optimizer.step();
	}
}

bool NeuralNetwork::load(const std::string& inputFilename) {
	try {
		serialize::InputArchive inputArchive;
		torch::Device device = torch::cuda::is_available() ? torch::Device(torch::kCUDA) : torch::Device(torch::kCPU);
		inputArchive.load_from(inputFilename, device);
		net.load(inputArchive);
	} catch (...) {
		return false;
	}
	
	return true;
}

bool NeuralNetwork::save(const std::string& outputFilename) const {
	try {
		serialize::OutputArchive outputArchive;
		net.save(outputArchive);
		outputArchive.save_to(outputFilename);
	} catch (...) {
		return false;
	}

	return true;
}