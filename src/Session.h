#ifndef SESSION_H
#define SESSION_H

#include <memory>

#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "boostUtils.h"

namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
	/**
	 * @brief Creates a session with the given socket
	 * @param socket socket used to communicate with client
	 * @param handleRequest function used to act on request
	 */
	explicit Session(tcp::socket socket, std::string (*handleRequest)(std::string));

	/**
	 * @brief Starts up
	 */
	void run();
private:
	/*
	 * @brief Socket used to communicate with client
	 */
	tcp::socket socket;
	/**
	 * @brief Buffer used to hold incoming data
	 */
	beast::flat_buffer buffer;
	/**
	 * @brief Used to hold requests
	 */
	http::request<http::string_body> request;
	/**
	 * @brief Function used to act on a request body and return a response body
	 */
	std::string (*handleRequest)(std::string);

	/**
	 * @brief Sets up asynchronous waiting for request
	 */
	void doRequest();

	/**
	 * @brief Acts on request
	 * @param errorCode error code if errored
	 * @param bytesTransferred number of bytes transferred 
	 */
	void onRequest(const beast::error_code& errorCode, std::size_t bytesTransferred);

	/**
	 * @brief Sets up asynchronous responding to request
	 * @param response response to send
	 */
	void doResponse(http::response<http::string_body> response);

	/**
	 * @brief Acts on response
	 * @param errorCode error code if errored
	 * @param bytesTransferred number of bytes transferred 
	 */
	void onResponse(const beast::error_code& errorCode, std::size_t bytesTransferred);
};

#endif
