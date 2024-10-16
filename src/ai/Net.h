#ifndef NET_H
#define NET_H

#include <torch/torch.h>

#include <vector>

#include "GameStateData.h"

#include "../game/GameStateConstants.h"

using namespace torch;

struct NetImpl : public nn::Module {
	nn::Conv3d conv1, conv2, conv3, conv4, convP1, convV1;
	nn::BatchNorm3d bn1, bn2, bn3, bn4, bnP1, bnV1;
	nn::Linear fcP1, fcP2, fcV1, fcV2;
	nn::BatchNorm1d fcBnP1, fcBnV1;

	/**
	 * @brief Constructor which initializes neural net layers
	 */
	NetImpl() :
			conv1(nn::Conv3dOptions(1, 64, 3).stride(1).padding(1)),
			conv2(nn::Conv3dOptions(64, 64, 3).stride(1).padding(1)),
			conv3(nn::Conv3dOptions(64, 64, 3).stride(1).padding(1)),
			conv4(nn::Conv3dOptions(64, 64, 3).stride(1).padding(1)),
			convP1(nn::Conv3dOptions(64, 64, 3).stride(1).padding(1)),
			convV1(nn::Conv3dOptions(64, 64, 3).stride(1).padding(1)),
			bn1(64), bn2(64), bn3(64), bn4(64), bnP1(64), bnV1(64),
			fcP1(64 * GAME_STATE_DATA_LENGTH, 512), fcV1(64 * GAME_STATE_DATA_LENGTH, 512), fcP2(512, NUM_MOVES), fcV2(512, 1),
			fcBnP1(512), fcBnV1(512)
	{
		register_module("conv1",conv1);
		register_module("conv2",conv2);
		register_module("conv3",conv3);
		register_module("conv4",conv4);
		register_module("convP1",convP1);
		register_module("convV1",convV1);
		register_module("bn1",bn1);
		register_module("bn2",bn2);
		register_module("bn3",bn3);
		register_module("bn4",bn4);
		register_module("bnP1",bnP1);
		register_module("bnV1",bnV1);
		register_module("fcP1",fcP1);
		register_module("fcP2",fcP2);
		register_module("fcV1",fcV1);
		register_module("fcV2",fcV2);
		register_module("fcBnP1",fcBnP1);
		register_module("fcBnV1",fcBnV1);
	}

	/**
	 * @brief Takes in tensor and returns move probabilities and value for each game state
	 * @param tensor tensor which contains game states
	 * @return vector containing a vector with lists of probabilities for moves and a vector with values for the corresponding game states
	 */
	std::vector<Tensor> forward(Tensor tensor){
		tensor = tensor.view({-1, 1, GAME_STATE_DATA_SIZE[0], GAME_STATE_DATA_SIZE[1], GAME_STATE_DATA_SIZE[2]}); //batchSize by GAME_STATE_DATA_SIZE[0] by GAME_STATE_DATA_SIZE[1] by GAME_STATE_DATA_SIZE[2]

		tensor = relu(bn1(conv1(tensor))); //batchSize by 64 by GAME_STATE_DATA_SIZE[1] by GAME_STATE_DATA_SIZE[2]
		tensor = relu(bn2(conv2(tensor))); //batchSize by 64 by GAME_STATE_DATA_SIZE[1] by GAME_STATE_DATA_SIZE[2]
		tensor = relu(bn3(conv3(tensor))); //batchSize by 64 by GAME_STATE_DATA_SIZE[1] by GAME_STATE_DATA_SIZE[2]
		tensor = relu(bn4(conv4(tensor))); //batchSize by 64 by GAME_STATE_DATA_SIZE[1] by GAME_STATE_DATA_SIZE[2]

		Tensor probabilities = relu(bnP1(convP1(tensor))); //batchSize by 64 by GAME_STATE_DATA_SIZE[1] by GAME_STATE_DATA_SIZE[2]
		probabilities = probabilities.view({-1, 64 * GAME_STATE_DATA_LENGTH}); //batchSize by 64 * GAME_STATE_DATA_LENGTH
		probabilities = dropout(relu(fcBnP1(fcP1(probabilities))), 0.1, is_training()); //batchSize by 512
		probabilities = fcP2(probabilities).nan_to_num(); //batchSize by NUM_MOVES

		Tensor value = relu(bnV1(convV1(tensor))); //batchSize by 64 by GAME_STATE_DATA_SIZE[1] by GAME_STATE_DATA_SIZE[2]
		value = value.view({-1, 64 * GAME_STATE_DATA_LENGTH}); //batchSize by 64 * GAME_STATE_DATA_LENGTH
		value = dropout(relu(fcBnV1(fcV1(value))), 0.1, is_training()); //batchSize by 512
		value = fcV2(value).nan_to_num(); //batchSize by 1

		return {log_softmax(probabilities, 1), value.tanh()};
	}
};
TORCH_MODULE(Net);

#endif
