#include <boost/asio/strand.hpp>

#include "Listener.h"

Listener::Listener(net::io_context& ioc, const tcp::endpoint& endpoint, std::string (*handleRequest)(std::string)) :
	ioc(ioc), acceptor(ioc), handleRequest(handleRequest) {
	beast::error_code errorCode;

	(void)acceptor.open(endpoint.protocol(), errorCode);
	if (errorCode) {
		printError(errorCode, "open");
		return;
	}

	(void)acceptor.set_option(net::socket_base::reuse_address(true), errorCode);
	if (errorCode) {
		printError(errorCode, "set_option");
		return;
	}

	(void)acceptor.bind(endpoint, errorCode);
	if (errorCode) {
		printError(errorCode, "bind");
		return;
	}

	(void)acceptor.listen(net::socket_base::max_listen_connections, errorCode);
	if (errorCode) {
		printError(errorCode, "listen");
		return;
	}
}

void Listener::run() {
	doAccept();
}

void Listener::doAccept() {
	acceptor.async_accept(net::make_strand(ioc), beast::bind_front_handler(&Listener::onAccept, shared_from_this()));
}

void Listener::onAccept(const beast::error_code& errorCode, tcp::socket socket) {
	if (errorCode) {
		printError(errorCode, "async_accept");
	} else {
		std::make_shared<Session>(std::move(socket), handleRequest)->run();
	}

	doAccept();
}