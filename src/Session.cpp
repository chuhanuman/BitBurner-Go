#include "Session.h"

Session::Session(tcp::socket socket, std::string (*handleRequest)(const std::string&)) :
	socket(std::move(socket)), handleRequest(handleRequest) {
}

void Session::run() {
	doRequest();
}

void Session::doRequest() {
    request = {};

    http::async_read(socket, buffer, request, beast::bind_front_handler(&Session::onRequest, shared_from_this()));
}

void Session::onRequest(const beast::error_code& errorCode, std::size_t bytesTransferred) {
    if(errorCode == http::error::end_of_stream) {
	    return;
    }

    if (errorCode) {
        return printError(errorCode, "async_read");
    }

    if (request.method() == http::verb::post) {
        http::response<http::string_body> response{http::status::ok, request.version()};
        response.set(http::field::content_type, "text/plain");
        response.keep_alive(request.keep_alive());
        response.body() = handleRequest(request.body());
        response.prepare_payload();
        return doResponse(response);
    }

    return doResponse(http::response<http::string_body>{http::status::bad_request, request.version()});
}


void Session::doResponse(http::response<http::string_body> response) {
    auto const sharedResponse = std::make_shared<http::response<http::string_body>>(std::move(response));
    http::async_write(socket, *sharedResponse, beast::bind_front_handler(&Session::onResponse, shared_from_this()));
}

void Session::onResponse(const beast::error_code& errorCode, std::size_t bytesTransferred) {
	if (errorCode) {
        return printError(errorCode, "async_write");
    }

	doRequest();
}