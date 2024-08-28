#include "GameState.h"
#include "MCTS.h"
#include "Listener.h"

#include <iostream>

int main() {
	auto const address = net::ip::make_address("0.0.0.0");
	constexpr unsigned short port = 8080;

	net::io_context ioc{1};
	std::make_shared<Listener>(ioc, tcp::endpoint{address, port}, [](std::string request) {
		std::cout << request << '\n';
		return request;
	})->run();

	ioc.run();

	return 0;
}
