#ifndef LISTENER_H
#define LISTENER_H

#include "Session.h"

class Listener : public std::enable_shared_from_this<Listener> {
public:
	/**
	 * @brief Creates a listener with the given IO context and endpoint
	 * @param ioc IO context to use
	 * @param endpoint endpoint to use
	 * @param handleRequest function used to act on request 
	 */
	Listener(net::io_context& ioc, const tcp::endpoint& endpoint, std::string (*handleRequest)(std::string));

	/**
	 * @brief Starts up
	 */
	void run();
private:
	/**
	 * @brief IO context to use
	 */
	net::io_context& ioc;
	/**
	 * @brief Socket used to accept connections
	 */
	tcp::acceptor acceptor;
	/**
	 * @brief Function used to act on a request body and return a response body
	 */
	std::string (*handleRequest)(std::string);

	/**
	 * @brief Sets up asynchronous accepting connections
	 */
	void doAccept();

	/**
	 * @brief Acts on accepting connection
	 * @param errorCode error code if errored
	 * @param socket new connection
	 */
	void onAccept(const beast::error_code& errorCode, tcp::socket socket);
};

#endif